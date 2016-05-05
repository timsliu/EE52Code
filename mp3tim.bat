asm86chk startup.asm
asm86chk initreg.asm
asm86chk mirq.asm
asm86chk timer0m.asm
asm86chk button.asm
asm86chk queue.asm
asm86chk displcd.asm
asm86chk converts.asm

asm86 startup.asm m1 ep db
asm86 initreg.asm m1 ep db
asm86 mirq.asm    m1 ep db
asm86 timer0m.asm m1 ep db
asm86 button.asm  m1 ep db
asm86 queue.asm   m1 ep db
asm86 displcd.asm m1 ep db
asm86 converts.asm m1 ep db

link86 startup.obj, initreg.obj, mirq.obj, timer0m.obj, button.obj to tim1.lnk
link86 queue.obj, displcd.obj, converts.obj to tim2.lnk
link86 fatutil.obj, ffrev.obj, keyupdat.obj, mainloop.obj to glen1.lnk
link86 playmp3.obj, stubfncs.obj, trakutil.obj, simide.obj to glen2.lnk

link86 tim1.lnk, tim2.lnk, glen1.lnk, glen2.lnk, glen3.lnk, ic86.lib to mp3tim.lnk


loc86 mp3tim.lnk to mp3tim NOIC AD(SM(CODE(1000H),DATA(400H),STACK(7000H)))