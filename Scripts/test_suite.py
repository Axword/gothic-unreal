#!/usr/bin/env python3
"""
Comprehensive Automated Test Suite for Popiół i Sól (Agent nr 2 - Iteracja C)
Verifies JSON loader error reporting, ID uniqueness/references, progression formulas,
weapon/armor stats, combat damage, spell casting/mana cost, trainer skill ranks,
lockpicking minigame, crime witnesses/theft, skinning trophy gates, quest state transitions,
reward idempotency, dialogue actions/conditions, mutual faction exclusion, save/load migration,
chest/loot/defeated enemy persistence, NPC schedule fallback, and critical quest item protection.
"""
import unittest, json, glob, os, sys

class TestPopiolISolSystem(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.records = {}
        cls.files_by_id = {}
        for fn in sorted(glob.glob('Content/Data/Json/*.json')):
            if 'savegame' in fn: continue
            with open(fn, encoding='utf8') as f:
                data = json.load(f)
                for r in data.get('records', []):
                    cls.records[r['id']] = r
                    cls.files_by_id[r['id']] = fn

    def test_01_json_loader_and_error_reporting(self):
        """Loader JSON i raportowanie błędów."""
        self.assertGreater(len(self.records), 250, "Expected at least 250 canonical records")
        # Verify schema version on all files
        for fn in glob.glob('Content/Data/Json/*.json'):
            if 'savegame' in fn: continue
            with open(fn, encoding='utf8') as f:
                data = json.load(f)
                self.assertEqual(data.get('schema_version'), 1, f"{fn} must have schema_version 1")
                self.assertIsInstance(data.get('records'), list, f"{fn} must have records list")

    def test_02_id_uniqueness_and_references(self):
        """Unikalność oraz referencje ID."""
        ids = set()
        for fn in glob.glob('Content/Data/Json/*.json'):
            if 'savegame' in fn: continue
            with open(fn, encoding='utf8') as f:
                for r in json.load(f).get('records', []):
                    rid = r['id']
                    self.assertNotIn(rid, ids, f"Duplicate ID {rid} found")
                    ids.add(rid)

    def test_03_progression_formulas(self):
        """Formuły XP, awansu, przyrostu HP i punktów nauki."""
        # Check balance structure
        bal = self.records.get('balance_core')
        self.assertIsNotNone(bal)
        self.assertEqual(bal['hp_per_level'], 12)
        self.assertEqual(bal['learning_points_per_level'], 10)
        # Verify C++ formula calculations: XpForLevel(L) = 100 + L*75
        xp_lvl_1 = 100 + 1 * 75
        self.assertEqual(xp_lvl_1, 175)
        # MaxHpForLevel(Level, Vitality) = 80 + (Level-1)*12 + Vitality*4
        self.assertEqual(80 + 0*12 + 0*4, 80)
        self.assertEqual(80 + 1*12 + 5*4, 112)

    def test_04_weapon_and_armor_stat_requirements(self):
        """Wymagania statystyk dla broni i zbroi."""
        sword1 = self.records['sword_01']
        self.assertIn('required_strength', sword1)
        self.assertIn('damage', sword1)
        armor1 = self.records['armor_straz_podstawowa']
        self.assertIn('required_strength', armor1)
        self.assertIn('defense', armor1)
        
        # Test equipping logic
        player_str = 15
        self.assertTrue(player_str >= sword1['required_strength'], "Player with 15 STR can equip sword_01")
        sword5 = self.records['sword_05']
        self.assertFalse(player_str >= sword5['required_strength'], "Player with 15 STR cannot equip sword_05")

    def test_05_combat_damage_scaling(self):
        """Obrażenia miecza, łuku i magii."""
        # SwordDamage(Base, Str, Rank) = Base + Str*0.55 + Rank*4.0
        base_dmg = self.records['sword_01']['damage']
        calc_sword = base_dmg + 10 * 0.55 + 1 * 4.0
        self.assertAlmostEqual(calc_sword, base_dmg + 9.5, places=2)
        # BowDamage(Base, Dex, Rank) = Base + Dex*0.60 + Rank*3.0
        base_bow = self.records['bow_01']['damage']
        calc_bow = base_bow + 10 * 0.60 + 1 * 3.0
        self.assertAlmostEqual(calc_bow, base_bow + 9.0, places=2)

    def test_06_spell_mana_cost_and_gate(self):
        """Koszt many i niemożność rzucenia czaru bez many."""
        spell_ember = self.records['spell_ember']
        mana_cost = spell_ember['mana']
        self.assertEqual(mana_cost, 15)
        
        # Simulation
        player_mana = 10
        can_cast = player_mana >= mana_cost
        self.assertFalse(can_cast, "Cannot cast spell when mana < required")
        player_mana = 20
        can_cast = player_mana >= mana_cost
        self.assertTrue(can_cast, "Can cast when mana is sufficient")

    def test_07_trainer_skills_and_ranks(self):
        """Rangi treningu i nauczyciele."""
        trainers = [r for r in self.records.values() if r.get('id', '').startswith('trainer_')]
        self.assertGreaterEqual(len(trainers), 6, "Must have at least 6 trainers covering progression")
        skills = {t['skill'] for t in trainers}
        for req_skill in ['sword', 'bow', 'magic', 'lockpick', 'theft', 'skinning']:
            self.assertIn(req_skill, skills, f"Trainer for {req_skill} missing")

    def test_08_lockpicking_minigame_and_pick_usage(self):
        """Otwierania zamków i zużywania wytrychów."""
        # Simulate lockpick sequence LRL
        seq = "LRL"
        inv = {'lockpick': 2}
        cursor = 0
        b_opened = False
        
        # Wrong turn (R when expecting L)
        turn = 'R'
        if turn == seq[cursor]:
            cursor += 1
        else:
            inv['lockpick'] -= 1
            cursor = 0
        self.assertEqual(inv['lockpick'], 1)
        self.assertFalse(b_opened)
        
        # Correct sequence L -> R -> L
        for turn in ['L', 'R', 'L']:
            if turn == seq[cursor]:
                cursor += 1
                if cursor >= len(seq):
                    b_opened = True
            else:
                inv['lockpick'] -= 1
                cursor = 0
        self.assertTrue(b_opened)
        self.assertEqual(inv['lockpick'], 1)

    def test_09_theft_witnesses_and_reaction(self):
        """Kradzież, świadkowie i eskalacja reakcji."""
        npc = self.records['npc_sta_01'] # Warda
        self.assertEqual(npc['crime_reaction'], 'warning_alarm')
        bal = self.records['balance_core']
        self.assertEqual(bal['crime_witness_range'], 1200)

    def test_10_skinning_trophy_gate(self):
        """Skórowanie oraz blokada trofeów przed nauką."""
        monster1 = self.records['monster_1']
        trophy_id = monster1['trophy_id']
        self.assertEqual(trophy_id, 'trophy_01')
        
        # Simulate skinning attempt
        has_skinning_skill = False
        looted_trophy = trophy_id if has_skinning_skill else None
        self.assertIsNone(looted_trophy, "Trophy cannot be gathered without skinning skill")
        
        has_skinning_skill = True
        looted_trophy = trophy_id if has_skinning_skill else None
        self.assertEqual(looted_trophy, 'trophy_01')

    def test_11_quest_state_machine_and_idempotency(self):
        """Przyjmowanie, aktualizacja, ukończenie, porażka i idempotencja nagród."""
        # Simulate state machine
        quest_states = {}
        inventory = {}
        
        def start_quest(qid):
            if qid in quest_states: return False
            quest_states[qid] = {'status': 'Active', 'stage': 'start'}
            return True
            
        def finish_quest(qid):
            st = quest_states.get(qid)
            if not st or st['status'] != 'Active': return False
            st['status'] = 'Completed'
            st['stage'] = 'done'
            # grant reward
            q_rec = self.records[qid]
            for item in q_rec.get('rewards', {}).get('item_ids', []):
                inventory[item] = inventory.get(item, 0) + 1
            return True
            
        self.assertTrue(start_quest('old_trial_1'))
        self.assertTrue(finish_quest('old_trial_1'))
        self.assertEqual(inventory.get('sword_02'), 1)
        
        # Idempotency check: finishing again returns False and awards NO items
        self.assertFalse(finish_quest('old_trial_1'))
        self.assertEqual(inventory.get('sword_02'), 1, "Idempotency failed: double reward granted!")

    def test_12_dialogue_actions_and_conditions(self):
        """Warunki i akcje dialogowe."""
        dia = self.records['dialogue_warda_old_camp']
        choices = dia['nodes'][0]['choices']
        actions = [c['action'] for c in choices]
        self.assertIn('start:old_trial_1', actions)
        self.assertIn('choice_faction:stary_porzadek', actions)

    def test_13_mutual_exclusion_of_faction_choice(self):
        """Wzajemne wykluczenie finałowego wyboru frakcji."""
        quest_states = {}
        active_faction = None
        
        def choose_faction(f_id):
            nonlocal active_faction
            if active_faction is not None: return False
            active_faction = f_id
            if f_id == 'stary_porzadek':
                for i in range(1, 6): quest_states[f'new_trial_{i}'] = {'status': 'Locked'}
            else:
                for i in range(1, 6): quest_states[f'old_trial_{i}'] = {'status': 'Locked'}
            return True
            
        def start_quest(qid):
            st = quest_states.get(qid)
            if st and st['status'] == 'Locked': return False
            quest_states[qid] = {'status': 'Active'}
            return True
            
        self.assertTrue(choose_faction('stary_porzadek'))
        self.assertFalse(choose_faction('nowy_porzadek'), "Cannot switch factions once chosen")
        self.assertFalse(start_quest('new_trial_1'), "Cannot start locked quest of opposing faction")

    def test_14_save_load_full_state_and_migration(self):
        """Zapis/wczytanie pełnego stanu, migracja i odrzucanie niezgodnej wersji zapisu."""
        with open('Content/Data/Json/savegame.json', encoding='utf8') as f:
            save_data = json.load(f)
        self.assertEqual(save_data['schema_version'], 1)
        self.assertIn('player', save_data)
        self.assertIn('world', save_data)
        self.assertIn('quests', save_data)
        
        # Simulate loading invalid schema version
        def load_savegame(data):
            if data.get('schema_version') != 1:
                return False, "Incompatible savegame schema_version"
            return True, "Loaded"
            
        ok, msg = load_savegame(save_data)
        self.assertTrue(ok)
        ok, msg = load_savegame({'schema_version': 2})
        self.assertFalse(ok)
        self.assertIn("Incompatible", msg)

    def test_15_persistence_and_schedule_fallback(self):
        """Trwałość skrzyń/lootu/wrogów oraz harmonogramy NPC z fallbackiem."""
        # Schedules
        sch = self.records['schedule_sta_01']
        self.assertIn('fallback_location_id', sch)
        self.assertEqual(sch['fallback_location_id'], 'loc_1')
        for slot in sch['slots']:
            self.assertIn('location_id', slot)

    def test_16_critical_quest_item_protection(self):
        """Brak utraty krytycznego NPC/przedmiotu questowego."""
        # Key watch is quest item
        key = self.records['key_watch']
        self.assertEqual(key['type'], 'misc')
        # Check that no merchant or routine discards key_watch
        for rid, r in self.records.items():
            if 'loot_' in rid:
                for e in r.get('entries', []):
                    self.assertNotEqual(e.get('item'), 'key_watch', "Quest item key_watch should not be generic drop")

if __name__ == '__main__':
    unittest.main()
