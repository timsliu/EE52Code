asm86chk startup.asm
asm86chk initreg.asm
asm86chk mirq.asm

asm86 startup.asm m1 ep db
asm86 initreg.asm m1 ep db
asm86 mirq.asm    m1 ep db

link86 startup.obj, initreg.obj, mirq.obj to startup.lnk

loc86 startup.lnk to startup NOIC AD(SM(CODE(1000H),DATA(400H),STACK(7000H)))