# FlexIO Camera External Resources

Date added: 2026-07-08

## Local copies

- `an-flexio_camera_rt1010/`
  - Source: https://github.com/nxp-appcodehub/an-flexio_camera_rt1010
  - NXP app-code reference for AN12686, using FlexIO to emulate a parallel camera interface on i.MX RT1010.
  - Key files for AVC porting:
    - `devices/MIMXRT1011/drivers/fsl_flexio.h`
    - `devices/MIMXRT1011/drivers/fsl_flexio.c`
    - `devices/MIMXRT1011/drivers/fsl_flexio_camera.h`
    - `devices/MIMXRT1011/drivers/fsl_flexio_camera.c`
    - `devices/MIMXRT1011/drivers/fsl_flexio_camera_edma.h`
    - `devices/MIMXRT1011/drivers/fsl_flexio_camera_edma.c`
    - `boards/evbmimxrt1010/demo_apps/flexio_camera/flexio_camera.c`
    - `boards/evbmimxrt1010/demo_apps/flexio_camera/ov7670/flexio_ov7670.c`
    - `boards/evbmimxrt1010/demo_apps/flexio_camera/ov7670/flexio_ov7670.h`

## Official references

- AN12686, "Using FlexIO to emulate Parallel Camera Interface on i.MX RT"
  - https://www.nxp.com/docs/en/application-note/AN12686.pdf
  - Closest source-level match to the local `an-flexio_camera_rt1010` repo.

- AN5275, "Using FlexIO for parallel Camera Interface"
  - https://www.nxp.com/docs/en/application-note/AN5275.pdf
  - Older Kinetis-era FlexIO camera app note, useful for register-level behavior.

- AN14836, "i.MX RT700 Camera Demo using FlexIO through eDMA"
  - https://www.nxp.com/docs/en/application-note/AN14836.pdf
  - Useful for the FlexIO plus eDMA capture model.
  - HTML documentation entry: https://docs.nxp.com/bundle/AN14836/page/topics/introduction.html

## Download note

Direct PDF downloads from `nxp.com/docs/en/application-note/*.pdf` returned an NXP "Page not available" HTML response from this environment, so the PDFs were not saved locally. The official links above remain the source of record.
