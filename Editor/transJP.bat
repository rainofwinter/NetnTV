del ..\Debug\*.qm
del ..\Release\*.qm

lrelease ja_JP.ts -qm trans.qm
lrelease qt_ja_JP.ts -qm base_trans.qm

copy *.qm ..\Debug
copy *.qm ..\Release



