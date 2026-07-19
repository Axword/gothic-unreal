#!/usr/bin/env python3
"""Generate original low-poly OBJ source meshes for Unreal import. No third-party geometry."""
from pathlib import Path
OUT=Path('Content/Art/SourceModels'); OUT.mkdir(parents=True,exist_ok=True)
MTL='''newmtl Iron\nKd 0.28 0.30 0.30\nnewmtl Rust\nKd 0.38 0.16 0.08\nnewmtl Wood\nKd 0.25 0.14 0.06\nnewmtl Cloth\nKd 0.16 0.25 0.22\nnewmtl Flesh\nKd 0.33 0.38 0.31\nnewmtl Bone\nKd 0.62 0.57 0.43\nnewmtl Magic\nKd 0.20 0.52 0.70\n''';(OUT/'pis_materials.mtl').write_text(MTL)
def mesh(name,parts,mat='Iron'):
 v=[];f=[];use=[]
 def box(x,y,z,sx,sy,sz,m=mat):
  if isinstance(sz,str): m=sz; sz=sy
  k=len(v)+1; v.extend([(x+dx*sx,y+dy*sy,z+dz*sz) for dx in(-1,1)for dy in(-1,1)for dz in(-1,1)])
  f.extend([(k,k+1,k+3,k+2),(k+4,k+6,k+7,k+5),(k,k+4,k+5,k+1),(k+2,k+3,k+7,k+6),(k,k+2,k+6,k+4),(k+1,k+5,k+7,k+3)]);use.extend([m]*6)
 def cone(x,y,z,r,h,m=mat):
  k=len(v)+1; v.extend([(x,y,z+h)]+[(x+r*(i%2*2-1),y+r*((i//2)*2-1),z) for i in range(4)])
  f.extend([(k,k+1,k+2),(k,k+2,k+4),(k,k+4,k+3),(k,k+3,k+1),(k+1,k+3,k+4,k+2)]);use.extend([m]*5)
 for p in parts:
  if p[0]=='b':box(*p[1:])
  else:cone(*p[1:])
 lines=['mtllib pis_materials.mtl','o '+name]
 for q in v:lines.append('v %.3f %.3f %.3f'%q)
 last=None
 for q,m in zip(f,use):
  if m!=last:lines.append('usemtl '+m);last=m
  lines.append('f '+' '.join(map(str,q)))
 (OUT/(name+'.obj')).write_text('\n'.join(lines)+'\n')
# weapons
for i in range(1,21):
 L=1.0+(i%5)*.12; W=.07+(i%4)*.018
 mesh(f'sword_{i:02}', [('b',0,0,L*.45,W,W,'Iron'),('b',0,0,-.15,.28,.10,'Wood'),('b',0,0,-.36,.10,.10,'Rust'),('c',0,0,L*.92,W*1.5,.20,'Iron')])
for i in range(1,11):
 h=1.1+i*.05
 mesh(f'bow_{i:02}', [('b',-.16,0,0,.035,.035,h*.50,'Wood'),('b',.16,0,0,.035,.035,h*.50,'Wood'),('b',0,0,0,.02,.02,h*.68,'Bone'),('b',0,0,-.12,.08,.06,.13,'Wood')])
# armor and characters
for n in ['armor_straz_podstawowa','armor_straz_lepsza','armor_wolni_podstawowa','armor_wolni_lepsza']:
 mesh(n,[('b',0,0,1.05,.40,.22,'Cloth'),('b',0,0,1.55,.24,.22,'Flesh'),('b',-.52,0,1.1,.12,.12,.48,'Cloth'),('b',.52,0,1.1,.12,.12,.48,'Cloth'),('b',-.20,0,.32,.16,.16,.38,'Cloth'),('b',.20,0,.32,.16,.16,.38,'Cloth')])
# creatures
for n in ['wilczak_solny','krabnik','topielec','skalnik','mroczek','plujec_bagienny']:
 parts=[('b',0,0,.65,.58,.32,.30,'Flesh'),('b',0,0,1.18,.27,.25,.27,'Flesh'),('b',-.60,0,.38,.12,.12,.42,'Bone'),('b',.60,0,.38,.12,.12,.42,'Bone')]
 if n=='krabnik':parts += [('b',-.85,0,.62,.45,.12,.10,'Bone'),('b',.85,0,.62,.45,.12,.10,'Bone')]
 if n=='mroczek':parts += [('b',-1.0,0,1.1,.7,.04,.35,'Cloth'),('b',1.0,0,1.1,.7,.04,.35,'Cloth')]
 if n=='plujec_bagienny':parts += [('c',0,0,1.42,.20,.45,'Magic')]
 mesh(n,parts)
# plants & environment
for i in range(1,11):mesh(f'plant_{i:02}', [('b',0,0,.35,.05,.05,.38,'Wood'),('c',0,0,.55,.22+(i%3)*.04,.30,'Cloth')])
for n,parts in {
 'house_rygiel':[('b',0,0,1.3,1.4,1.1,1.3,'Wood'),('c',0,0,2.4,1.65,1.3,'Rust')],
 'house_brzeg':[('b',0,0,1,1.5,.9,1,'Wood'),('c',0,0,1.9,1.7,1.0,'Cloth')],
 'rock_large':[('c',0,0,0,1.1,1.5,'Iron')], 'tree_pine':[('b',0,0,1.2,.13,.13,1.2,'Wood'),('c',0,0,1.8,.9,1.8,'Cloth')],
 'chest':[('b',0,0,.4,.65,.4,.4,'Wood'),('b',0,.18,.85,.65,.16,.12,'Rust')],
 'barrel':[('b',0,0,.45,.35,.35,.45,'Wood')], 'dock_post':[('b',0,0,.8,.14,.14,.8,'Wood')],
 'palisade':[('b',0,0,1,.16,.16,1,'Wood'),('c',0,0,2,.18,.35,'Wood')],
 'swamp_log':[('b',0,0,.25,1.2,.20,.20,'Wood')], 'quarry_cart':[('b',0,0,.35,.8,.45,.25,'Iron')]
}.items():mesh(n,parts)
print('Generated',len(list(OUT.glob('*.obj'))),'OBJ meshes')
