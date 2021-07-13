#pragma once

#define NEW new
#define DELETE_SAFE(p) do { if (p) delete p; p = nullptr; } while(false)
#define DELETE_VEC(p) do { if (p) delete[] p; p = nullptr; } while(false)
#define DELETE_ONLY(p) do { if (p) delete p; } while(false)

