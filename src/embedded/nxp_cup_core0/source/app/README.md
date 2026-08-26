# Adding Student Code

The supplied files have one job each:

- `test_mode.c` automatically dispatches the active TEST-jumper page;
- `camera_test.c`, `vision_test.c`, and `motor_test.c` implement those pages;
- `race_mode.c` is the competition callback.

Students normally experiment in `vision_test.c` and `race_mode.c`. The camera
and motor pages are small public-API examples, while the dispatcher is normally
left unchanged.

Put each additional module directly in this folder:

```text
app/
  my_algorithm.c
  my_algorithm.h
```

Include it from the page or race module that uses it:

```c
#include "my_algorithm.h"
```

Then run the normal build. Every `.c` file directly in `app` is discovered and
compiled automatically; `.h` files need no build-system change.

```powershell
.\src\embedded\build.ps1
```

Keep board drivers and safety changes outside this folder. Student modules use
the public functions and types from `nxp_cup.h`.
