#!/usr/bin/env python3
"""Dependency-free CI validator for canonical JSON data."""
import glob,json,sys,re
records={}; errors=[]
for fn in glob.glob('Content/Data/Json/*.json'):
 try: root=json.load(open(fn,encoding='utf8'))
 except Exception as e: errors.append(f'{fn}: invalid JSON: {e}');continue
 if fn.endswith('savegame.json'): continue
 if root.get('schema_version')!=1 or not isinstance(root.get('records'),list): errors.append(f'{fn}: expected schema_version=1 and records');continue
 for x in root['records']:
  ident=x.get('id')
  if not isinstance(ident,str) or not re.match(r'^[a-z0-9_]+$',ident): errors.append(f'{fn}: invalid id {ident!r}')
  elif ident in records: errors.append(f'{fn}: duplicate ID {ident}')
  else: records[ident]=x
REF_KEYS={'npc_id','monster_id','location_id','schedule_id','speaker_id','quest_id','item_id'}
def refs(value, path=''):
 if isinstance(value,dict):
  for k,v in value.items():
   if k in REF_KEYS and isinstance(v,str) and v: yield k,v,path+'/'+k
   yield from refs(v,path+'/'+k)
 elif isinstance(value,list):
  for i,v in enumerate(value): yield from refs(v,path+f'[{i}]')
for ident,x in records.items():
 for key,val,path in refs(x):
  if val not in records: errors.append(f'{ident}: missing reference {path}={val}')
print(f'validated {len(records)} records; '+('FAILED' if errors else 'OK'))
print('\n'.join(errors)); sys.exit(bool(errors))
