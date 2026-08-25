# Adding Student Code

Put each additional module directly in this folder:

```text
app/
  my_algorithm.c
  my_algorithm.h
```

Include it from `test_mode.c` or `race_mode.c`:

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
