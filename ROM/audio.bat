asm86chk askel.asm
asm86chk initreg.asm
asm86chk audios.asm
asm86chk amirq.asm
asm86chk dram.asm
asm86chk timer1m.asm


asm86 askel.asm    m1 ep db
asm86 initreg.asm m1 ep db
asm86 audios.asm  m1 ep db
asm86 amirq.asm    m1 ep db
asm86 dram.asm     m1 ep db
asm86 timer1m.asm   m1 ep db


link86 askel.obj, initreg.obj, audios.obj, amirq.obj, dram.obj, timer1m.obj to audio.lnk

loc86 audio.lnk to audio NOIC AD(SM(CODE(1000H),DATA(400H),STACK(7000H)))