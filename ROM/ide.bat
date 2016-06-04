asm86chk skel.asm
asm86chk initreg.asm
asm86chk ide.asm


asm86 skel.asm    m1 ep db
asm86 initreg.asm m1 ep db


link86 skel.obj, initreg.obj to ide.lnk

loc86 ide.lnk to ide NOIC AD(SM(CODE(1000H),DATA(400H),STACK(7000H)))