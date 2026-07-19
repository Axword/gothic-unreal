#!/usr/bin/env python3
"""Dependency-free CI validator for canonical JSON data."""
import glob, json, sys, re

records = {}
records_by_file = {}
errors = []

for fn in sorted(glob.glob('Content/Data/Json/*.json')):
    try:
        root = json.load(open(fn, encoding='utf8'))
    except Exception as e:
        errors.append(f'{fn}: invalid JSON: {e}')
        continue
    if fn.endswith('savegame.json'):
        continue
    if root.get('schema_version') != 1 or not isinstance(root.get('records'), list):
        errors.append(f'{fn}: expected schema_version=1 and records array')
        continue
    for x in root['records']:
        ident = x.get('id')
        if not isinstance(ident, str) or not re.match(r'^[a-z0-9_]+$', ident):
            errors.append(f'{fn}: invalid id {ident!r}')
        elif ident in records:
            errors.append(f'{fn}: duplicate ID {ident} (also in {records_by_file.get(ident)})')
        else:
            records[ident] = x
            records_by_file[ident] = fn

# Recursive nested reference check
def check_val(rid, key, val):
    if isinstance(val, dict):
        for k, v in val.items():
            check_val(rid, k, v)
    elif isinstance(val, list):
        for v in val:
            check_val(rid, key, v)
    elif isinstance(val, str) and val:
        if key.endswith('_id') or key in ['item', 'speaker_id', 'target_id', 'schedule_id', 'trophy_id', 'loot_table_id']:
            # check if reference exists
            if val not in records and val != 'chest_prototype':
                errors.append(f'{rid}: missing reference {key}={val!r}')
        elif key == 'action':
            if val.startswith('start:'):
                q = val.split(':')[1]
                if q not in records:
                    errors.append(f'{rid}: missing quest in action {val!r}')
            elif val.startswith('set_stage:'):
                parts = val.split(':')
                if len(parts) >= 3 and parts[1] not in records:
                    errors.append(f'{rid}: missing quest in action {val!r}')

for rid, x in records.items():
    fn = records_by_file[rid]
    # check all nested fields for references
    for key, val in x.items():
        check_val(rid, key, val)
        
    # Domain specific checks
    if 'swords.json' in fn:
        for req_field in ['damage', 'required_strength', 'value', 'asset_path']:
            if req_field not in x:
                errors.append(f'{rid}: sword missing required field {req_field}')
    elif 'bows.json' in fn:
        for req_field in ['damage', 'required_dexterity', 'value', 'asset_path']:
            if req_field not in x:
                errors.append(f'{rid}: bow missing required field {req_field}')
    elif 'armors.json' in fn:
        for req_field in ['defense', 'required_strength', 'asset_path']:
            if req_field not in x:
                errors.append(f'{rid}: armor missing required field {req_field}')
    elif 'monsters.json' in fn:
        for req_field in ['hp', 'damage', 'xp', 'trophy_id', 'loot_table_id']:
            if req_field not in x:
                errors.append(f'{rid}: monster missing required field {req_field}')
    elif 'spells.json' in fn:
        for req_field in ['damage', 'mana', 'cast_time']:
            if req_field not in x:
                errors.append(f'{rid}: spell missing required field {req_field}')
    elif 'npc_schedules.json' in fn:
        if 'slots' not in x or not isinstance(x['slots'], list) or not x['slots']:
            errors.append(f'{rid}: schedule missing or empty slots list')
        for slot in x.get('slots', []):
            if 'location_id' not in slot and 'marker' not in slot:
                errors.append(f'{rid}: schedule slot missing location_id or marker')
    elif 'quests_' in fn:
        if 'stages' not in x or not isinstance(x['stages'], list) or not x['stages']:
            errors.append(f'{rid}: quest missing or empty stages list')
        # check against dummy text
        for st in x.get('stages', []):
            if st.get('text') == 'Zbierz ślad i wróć.':
                errors.append(f'{rid}: quest stage contains dummy placeholder text')

print(f'validated {len(records)} records; ' + ('FAILED' if errors else 'OK'))
if errors:
    print('\n'.join(errors))
    sys.exit(1)
sys.exit(0)
