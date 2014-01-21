#if _MSC_VER >= 1600 /* VC 10 or greater, hypot is not available but needed by old GSL */

double hypot(double x, double y)
{
    extern double _hypot(double x, double y);
    return _hypot(x, y);
}

#endif
