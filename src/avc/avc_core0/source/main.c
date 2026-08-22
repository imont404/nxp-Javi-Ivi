

#include "avc__io.h"
#include "avc__line_processor.h"

#if CONFIG__USB_DEBUG_STREAM_ENABLE
#include "avc_usb_debug_stream.h"
#endif

#if CONFIG__MOTOR_ENCODER_BACKEND == MOTOR_ENCODER_BACKEND_QDC
#include "avc__motor_encoder_qdc.h"
#endif


volatile uint16_t bat;


float left_intensity;
float right_intensity;
float servo_position;

bool testmode__motors_enable = 0;

/*
 * Test mode gates the LCD refresh path. Normally it follows the TEST switch;
 * diagnostic builds can force it so the display path can be measured without
 * someone at the bench. See CONFIG__FORCE_TEST_MODE.
 */
/*
 * Scope marker bracketing the whole per-frame LCD dump. See
 * CONFIG__DISPLAY_SCOPE_MARKER_ENABLE. Independent of the cycle counter, which
 * avc__next_frame() resets.
 */
#if CONFIG__DISPLAY_SCOPE_MARKER_ENABLE
#define AVC_SCOPE_DUMP_BEGIN() GPIO_PinWrite(GPIO4, 1U, 1U)
#define AVC_SCOPE_DUMP_END()   GPIO_PinWrite(GPIO4, 1U, 0U)
#else
#define AVC_SCOPE_DUMP_BEGIN() do { } while (0)
#define AVC_SCOPE_DUMP_END()   do { } while (0)
#endif

#if CONFIG__FORCE_TEST_MODE
#define AVC_TEST_MODE_ACTIVE() (1)
#else
#define AVC_TEST_MODE_ACTIVE() (GPIO_PinRead(GPIO3, TEST_SW_PIN) == 0)
#endif


uint32_t line_to_process;

uint8_t pot_text[64];

volatile bool next_frame_ready;

float alpha, gamma, beta;

#if CONFIG__MOTOR_ENCODER_BACKEND == MOTOR_ENCODER_BACKEND_QDC
/*
 * Wheel speed, sampled on a fixed interval rather than per frame so the rate is
 * measured over a known window. Forward reads positive - see
 * CONFIG__MOTOR_ENCODER_INVERT_M0/M1.
 */
#define AVC_ENCODER_SAMPLE_MS (100U)

static avc_motor_encoder_sample_t encoder_samples[AVC_MOTOR_ENCODER_COUNT];
static uint32_t encoder_sample_tick;

static void avc__encoder_service(void)
{
    if (e_tick__timeout(&encoder_sample_tick, AVC_ENCODER_SAMPLE_MS))
    {
        avc__motor_encoder_qdc_sample(AVC_ENCODER_SAMPLE_MS, encoder_samples);
    }
}

/* Wheel speed in RPM. Positive is forward. */
float avc__wheel_rpm(avc_motor_encoder_id_t wheel)
{
    return (float)encoder_samples[wheel].rpm_milli / 1000.0f;
}

/* Wheel speed in metres per second at the contact patch. Positive is forward. */
float avc__wheel_velocity_mps(avc_motor_encoder_id_t wheel)
{
    const float circumference_m =
        3.14159265f * ((float)CONFIG__MOTOR_ENCODER_WHEEL_DIAMETER_MM / 1000.0f);

    return (avc__wheel_rpm(wheel) / 60.0f) * circumference_m;
}
#endif

eGFX_ImagePlane top_info;

eGFX_ImagePlane camera_view;

avc_color_features_t line_color[CONFIG__CAMERA_RESOLUTION_X];

uint8_t black_filter[CONFIG__CAMERA_RESOLUTION_X];

uint8_t red_filter[CONFIG__CAMERA_RESOLUTION_X];

uint8_t top_info_buffer[eGFX_CALCULATE_16BPP_IMAGE_STORAGE_SPACE_SIZE(320,40)];

uint16_t *raw_camera_frame;

//Some variables for tracking execution time
uint32_t algorithm_timer;

uint32_t frame_timer;

bool pots_reset;

bool motors_on;

void avc__update_overlay();

/*
 * This is called by the camera interface when we get a new frame.
 * This executes in an interrupt context.
 */


void avc__next_frame(uint16_t *buf)
{
	next_frame_ready = true;

	raw_camera_frame = buf;

	frame_timer = CYCLE_COUNTER;

	//Reset the CPU Cycle counter to measure the next frame
	CYCLE_COUNTER = 0;

	algorithm_timer = 0;
}

int main(void)
{

	avc__init();

#if CONFIG__MOTOR_ENCODER_DIAG_ENABLE
    avc__motor_encoder_qdc_diag_run();
#endif

#if CONFIG__MOTOR_ENCODER_BACKEND == MOTOR_ENCODER_BACKEND_QDC
    encoder_sample_tick = e_tick__get_ms();
#endif

	//Initialize the ARM CPU cycle counter for measuring performance


	/*
	 * Initialize two image plane structures
	 *
	 * one for the graphic overlay at the top and the other for the camera data
	 */
	top_info.Data = top_info_buffer;
	top_info.SizeX = 320;
	top_info.SizeY = 40;
	top_info.Type = eGFX_IMAGE_PLANE_16BPP_RGB565;

	camera_view.SizeX = CONFIG__CAMERA_RESOLUTION_X;
	camera_view.SizeY = CONFIG__CAMERA_RESOLUTION_Y;
	camera_view.Type = eGFX_IMAGE_PLANE_16BPP_RGB565;



    while (1)
    {
        avc_camera__service();

#if CONFIG__MOTOR_ENCODER_BACKEND == MOTOR_ENCODER_BACKEND_QDC
        avc__encoder_service();
#endif

#if CONFIG__USB_DEBUG_STREAM_ENABLE
        avc_usb_debug_stream__service();
#endif

        bat = avc__read_battery_voltage();
        //Alpha, beta and gamma will be -1.0f to 1.0f

        // Test mode enable
        if(AVC_TEST_MODE_ACTIVE())
        {   

            if(next_frame_ready)
            {
            	next_frame_ready=false;
				uint16_t *frame = raw_camera_frame;
				camera_view.Data = (uint8_t *)frame;
				avc_camera__prepare_frame(frame);

            	   if(button__up(&center_btn))
            	   {
            		   pots_reset = true;

            	       if(testmode__motors_enable == 0)
            	       {
            	           testmode__motors_enable = 1;

            	       }

            	       else if(testmode__motors_enable == 1)
            	       {
            	           testmode__motors_enable = 0;
            	           avc__disable_motor_control();

            	       }
            	   }

            	   if(testmode__motors_enable == 1)
            	    {
            		   if(pots_reset == true)
            		   {
            			   motors_on = false;
            			   //make sure potentiometers are at midpoint before enabling motors
            			   if((avc__read_alpha() <= 0.55 && avc__read_alpha()> 0.45) &&
            			     (avc__read_beta() <= 0.55 && avc__read_beta()> 0.45) &&
							 (avc__read_gamma() <= 0.55 && avc__read_gamma()> 0.45))
            			   {
            				   pots_reset = false;
            				   avc__enable_motor_control();
            			   }
            		   }
            		   else
            		   {
            			   motors_on = true;
						   //rescale potentiometer 0-1.0f to -1.0f to 1.0f;
						   left_intensity = (avc__read_alpha() * 2.0f)-1.0;
						   right_intensity = (avc__read_gamma() * 2.0f)-1.0f;
						   servo_position = (avc__read_beta() * 2.0f)-1.0f;

						   avc__set_motor_pwm(left_intensity, right_intensity);
						   avc__set_servo(servo_position);
            		   }
            	    }
            	   else
            	   {
            		   //Use the alpha pot to select which line to process

            		   line_to_process = 50.0f + (avc__read_alpha()*150.0f);

            		   avc__set_servo(0);
            	   }

            	/*
            	 *raw_camera_frame is a pointer to 16-bit
            	 * frame data in RGB565 format.
            	 *  You can use it like an array
            	 *  raw_camera_frame[5] would be the 6th pixel on the 1st line
            	 *
            	 *  Since lines are 320 pixel long, you could do this:
            	 *
            	 *  raw_camera_frame [5 + 2*CONFIG__CAMERA_RESOLUTION_X];
            	 *
            	 *  This would get you pixel 6 on the 3rd line
            	 */

					uint32_t line = line_to_process;

					avc__convert_rgb565_to_yhsv(&frame[line * CONFIG__CAMERA_RESOLUTION_X],
											 line_color,
											 CONFIG__CAMERA_RESOLUTION_X);

					/*
					 * Y, H, S, and V are uint8 values from one RGB565-indexed table
					 * read per pixel. Thresholding remains visible here for teaching;
					 * the table contains no track- or obstacle-specific classes.
					 */
					const uint8_t black_y_threshold =
						(uint8_t)(avc__read_beta() * 255.0f);
					const uint8_t red_saturation_min = 77u; /* 0.30 x 255. */
					const uint8_t red_value_min = 64u;
					const uint8_t red_hue_low_max = 21u;   /* 30 degrees. */
					const uint8_t red_hue_high_min = 235u; /* 330 degrees. */

					for (int x = 0; x < CONFIG__CAMERA_RESOLUTION_X; x++)
					{
						const avc_color_features_t color = line_color[x];

						black_filter[x] = (color.y <= black_y_threshold) ? 1u : 0u;
						red_filter[x] =
							((color.s > red_saturation_min) &&
							 (color.v > red_value_min) &&
							 ((color.h >= red_hue_high_min) ||
							  (color.h <= red_hue_low_max))) ? 1u : 0u;
					}



				for(int x=0;x<CONFIG__CAMERA_RESOLUTION_X;x++)
				{
					if(black_filter[x])
					{
						eGFX_DrawV_Line(&camera_view, line-2, line+2, x, eGFX_COLOR_RGB888_TO_RGB565(0,255,0));
					}
					else
					{
						eGFX_DrawV_Line(&camera_view ,  line-1, line+1, x , eGFX_COLOR_RGB888_TO_RGB565(255,255,255));

					}
				}


            	avc__update_overlay();

#if CONFIG__USB_DEBUG_STREAM_ENABLE
				(void)avc_usb_debug_stream__publish_frame(frame);
#endif


            	/*
            	 * Dump the overlay graphic to the LCD
            	 *
            	 */

            	AVC_SCOPE_DUMP_BEGIN();
            	eGFX_DumpRaw((uint8_t *)top_info.Data,
                					  320*40*2,
                					  0,
                					  319,
                					  0,
                					  39);

             	/*
                 * Dump the camera view data
                 *
                 */
            	eGFX_DumpRaw((uint8_t *)camera_view.Data,
            					  320*200*2,
            					  0,
            					  319,
            					  40,
            					  39+200);

            	AVC_SCOPE_DUMP_END();

            }



        }//end test most
        else
        {
        	//This is where we put code if we are not it test mode
        	  if(button__up(&center_btn))
        	  {
        		  //Start the car!
        	  }

        }
    }

}

void avc__update_overlay()
{
	eGFX_ImagePlane_Clear(&top_info);





   sprintf(pot_text, "a:%.2f", avc__read_alpha());

   eGFX_DrawStringColored(&top_info,
   						pot_text,0,20,
						&FONT_10_14_1BPP,
						eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));

   sprintf(pot_text, "b:%.2f", avc__read_beta());
   eGFX_DrawStringColored(&top_info,
   						pot_text,120,20,
						&FONT_10_14_1BPP,
						eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));

   sprintf(pot_text, "g:%.2f", avc__read_gamma());
   eGFX_DrawStringColored(&top_info,
   						pot_text,255,20,
						&FONT_10_14_1BPP,
						eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));

#if CONFIG__MOTOR_ENCODER_BACKEND == MOTOR_ENCODER_BACKEND_QDC
   /*
    * Wheel speed. Positive is forward.
    *
    * The 320x40 band is already crowded, so this uses the small font and the
    * gaps rather than a row of its own:
    *   y 0-14, x 100-270 : motor status, FONT_10_14
    *   y 3-10,  x 3-48   : CPU percentage, FONT_5_7
    *   y 20-34           : pot values, FONT_10_14
    * That leaves y 11-18 free left of x=100, and the column right of x=270.
    * RPM for both wheels goes in the first, per-wheel m/s stacked in the second.
    *
    * Showing both wheels together is the point - they do not match at equal
    * drive, with M1 running several percent faster than M0. The whole band
    * wants a proper layout pass eventually.
    */
   /* Both wheels' RPM in the strip under the CPU readout. */
   sprintf(pot_text, "L%+.1f R%+.1f rpm",
           avc__wheel_rpm(AVC_MOTOR_ENCODER_M0),
           avc__wheel_rpm(AVC_MOTOR_ENCODER_M1));

   eGFX_DrawStringColored(&top_info,
                          pot_text,3,11,
                          &FONT_5_7_1BPP,
                          eGFX_COLOR_RGB888_TO_RGB565(0xFF,0xFF,0));

   /*
    * Ground speed per wheel, stacked in the free column right of the motor
    * status. Independent per wheel on purpose - the two do not match, and a
    * single combined figure would hide exactly the asymmetry worth seeing.
    */
   sprintf(pot_text, "L%+.2f", avc__wheel_velocity_mps(AVC_MOTOR_ENCODER_M0));

   eGFX_DrawStringColored(&top_info,
                          pot_text,274,1,
                          &FONT_5_7_1BPP,
                          eGFX_COLOR_RGB888_TO_RGB565(0xFF,0xFF,0));

   sprintf(pot_text, "R%+.2f", avc__wheel_velocity_mps(AVC_MOTOR_ENCODER_M1));

   eGFX_DrawStringColored(&top_info,
                          pot_text,274,10,
                          &FONT_5_7_1BPP,
                          eGFX_COLOR_RGB888_TO_RGB565(0xFF,0xFF,0));
#endif


   if(button__is_active(&left_btn))
   {
       eGFX_DrawStringColored(&camera_view,
   						"L_BTN",0,0,
						&FONT_10_14_1BPP,
						eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));
   }

   if(button__is_active(&right_btn))
   {
       eGFX_DrawStringColored(&camera_view,
   						"R_BTN",290,0,
						&FONT_10_14_1BPP,
						eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));
   }



   if(testmode__motors_enable == 1)
    {
	   if(motors_on)
	   {
      	eGFX_DrawStringColored(&top_info,
                    			"MOTORS ON",100,0,
        						&FONT_10_14_1BPP,
        						eGFX_COLOR_RGB888_TO_RGB565(0xFF,0,0));
	   }
	   else
	   {
	      	eGFX_DrawStringColored(&top_info,
	                    		"RESET POTS to 0.5",100,0,
	        					&FONT_10_14_1BPP,
	        					eGFX_COLOR_RGB888_TO_RGB565(0xFF,0xFF,0));
	   }
    }
   else
   {

     	eGFX_DrawStringColored(&top_info,
                   			"MOTORS OFF",100,0,
       						 &FONT_10_14_1BPP,
       						 eGFX_COLOR_RGB888_TO_RGB565(0,0x80,0));
   }


    //Use the CPU cycle counter to compute a % of CPU time per frame use for computations

	algorithm_timer = CYCLE_COUNTER;

    float cpu =  ((float)algorithm_timer / (float)frame_timer) * 100.0f;

	sprintf(pot_text, "CPU:%.1f%%", cpu) ;

	eGFX_DrawStringColored(&top_info,
	   						pot_text,3,3,
							&FONT_5_7_1BPP,
		 					eGFX_COLOR_RGB888_TO_RGB565(0,192,0));

}

