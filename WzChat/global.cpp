#include "global.h"

//强制让某个QWidget重新应用当前的QStyle
std::function<void(QWidget*)> repolish =[](QWidget *w){
    w->style()->unpolish(w);
    w->style()->polish(w);
};
