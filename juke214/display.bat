asm86chk startup.asm
asm86chk initreg.asm
asm86chk mirq.asm
asm86chk timer0m.asm
asm86chk displcd.asm
asm86chk button.asm
asm86chk queue.asm
asm86chk converts.asm

asm86 startup.asm m1 ep db
asm86 initreg.asm m1 ep db
asm86 mirq.asm    m1 ep db
asm86 timer0m.asm m1 ep db
asm86 displcd.asm m1 ep db
asm86 button.asm  m1 ep db
asm86 queue.asm   m1 ep db
asm86 converts.asm m1 ep db

link86 startup.obj, initreg.obj, mirq.obj, timer0m.obj to LCD1.lnk
link86 button.obj, displcd.obj, queue.obj, converts.obj to LCD2.lnk
link86 LCD1.lnk, LCD2.lnk to LCD.lnk
loc86 LCD.lnk to LCD NOIC AD(SM(CODE(1000H),DATA(400H),STACK(7000H)))