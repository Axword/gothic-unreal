#!/usr/bin/env python3
"""Creates original, dependency-free 128px PNG source icons for Unreal import.
These are intentionally simple production placeholders, not generated third-party art."""
import os,struct,zlib,json,glob
W=H=128; OUT='Content/Art/Icons'
PALETTES={'sword':(145,84,60),'bow':(130,101,55),'armor':(91,106,112),'plant':(68,120,76),'potion':(98,114,164),'trophy':(132,109,76),'misc':(156,145,102),'spell':(87,157,185),'monster':(126,69,75)}
def png(path,p):
 raw=b''.join(b'\0'+bytes(v for px in row for v in px) for row in p)
 def chunk(t,d):return struct.pack('>I',len(d))+t+d+struct.pack('>I',zlib.crc32(t+d)&0xffffffff)
 open(path,'wb').write(b'\x89PNG\r\n\x1a\n'+chunk(b'IHDR',struct.pack('>IIBBBBB',W,H,8,2,0,0,0))+chunk(b'IDAT',zlib.compress(raw,9))+chunk(b'IEND',b''))
def icon(kind,n,name):
 c=PALETTES[kind]; p=[[(27,30,32) for x in range(W)]for y in range(H)]
 def px(x,y,col):
  if 0<=x<W and 0<=y<H:p[y][x]=col
 # border and mottled backdrop
 for y in range(H):
  for x in range(W):
   shade=((x*7+y*11+n*19)%17)-8; px(x,y,tuple(max(0,min(255,a+shade)) for a in (38,42,43)))
 for x in range(W):px(x,0,(185,176,150));px(x,H-1,(185,176,150))
 for y in range(H):px(0,y,(185,176,150));px(W-1,y,(185,176,150))
 def rect(x0,y0,x1,y1,col):
  for y in range(y0,y1):
   for x in range(x0,x1):px(x,y,col)
 def line(x0,y0,x1,y1,col,w=3):
  steps=max(abs(x1-x0),abs(y1-y0));
  for i in range(steps+1):
   x=round(x0+(x1-x0)*i/steps);y=round(y0+(y1-y0)*i/steps)
   for yy in range(y-w,y+w+1):
    for xx in range(x-w,x+w+1):px(xx,yy,col)
 light=tuple(min(255,a+65) for a in c); dark=tuple(max(0,a-45) for a in c)
 if kind=='sword': line(35,93,89,27,light,5);line(30,98,48,104,dark,6);line(29,91,53,111,c,4);rect(78,21,94,40,light)
 elif kind=='bow': line(36,23,86,104,light,4);line(36,23,83,80,dark,2);line(41,30,88,105,dark,2);line(42,28,83,102,(220,215,190),1)
 elif kind=='armor': rect(40,37,88,101,c);rect(32,48,96,76,light);rect(48,27,80,42,dark);line(47,40,47,103,(200,190,160),2);line(81,40,81,103,(200,190,160),2)
 elif kind=='plant': line(64,105,65,36,light,4);line(63,67,32,49,c,7);line(66,80,98,59,c,7);rect(53,27,77,48,light)
 elif kind=='potion': rect(47,40,81,96,c);rect(53,26,75,43,light);rect(42,61,86,89,light);rect(52,23,76,28,(210,205,180))
 elif kind=='trophy': rect(40,46,89,94,c);line(43,52,26,26,light,4);line(85,52,103,25,light,4);rect(51,60,61,70,(220,210,180));rect(70,60,80,70,(220,210,180))
 elif kind=='misc': rect(34,39,94,91,c);rect(41,28,86,48,light);line(44,48,83,85,dark,3)
 elif kind=='spell':
  for r in range(38,5,-5):
   for y in range(64-r,64+r):
    for x in range(64-r,64+r):
     if (x-64)**2+(y-64)**2<r*r:px(x,y, light if r<20 else c)
 elif kind=='monster': rect(37,48,91,91,c);rect(45,34,83,57,light);rect(50,59,59,68,(230,220,190));rect(71,59,80,68,(230,220,190));line(43,50,28,34,dark,5);line(86,50,101,34,dark,5)
 png(os.path.join(OUT,'icon_'+name+'.png'),p)
for fn in glob.glob('Content/Data/Json/*.json'):
 d=json.load(open(fn,encoding='utf8'))
 for i,r in enumerate(d.get('records',[]),1):
  ident=r.get('id',''); typ=r.get('type','')
  kind=typ if typ in PALETTES else ('spell' if ident.startswith('spell_') else 'monster' if ident.startswith('monster_') else None)
  if kind: icon(kind,i,ident)
print('Generated',len(os.listdir(OUT)),'PNG icons')
