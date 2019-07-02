del ..\Debug\*.qm
del ..\Release\*.qm

lrelease ko_Kr.ts -qm trans.qm
lrelease qt_ko_KR.ts -qm base_trans.qm

copy *.qm ..\Debug
copy *.qm ..\Release



