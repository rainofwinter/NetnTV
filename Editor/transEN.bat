del ..\Debug\*.qm
del ..\Release\*.qm

lrelease us_En.ts -qm trans.qm
lrelease qt_us_EN.ts -qm base_trans.qm

copy *.qm ..\Debug
copy *.qm ..\Release
