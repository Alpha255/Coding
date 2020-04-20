using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

namespace BlackJack.Tat.Tools.Unity.AssetPreset
{
    public class ConfigEditor : EditorWindow
    {
        private Texture.PresetConfigItems mConfigItems;
        private Vector2 mScrollPosition;

        [MenuItem("Assets/BlackJack/AssetPresetConfig")]
        static public void ShowWindow()
        {
            EditorWindow.GetWindow<ConfigEditor>("Asset Preset Configuration").Show();
        }

        private Texture.PresetConfigItem OnItemGUI(Texture.PresetConfigItem item, ref bool remove)
        {
            GUILayout.BeginHorizontal("Box");
            GUILayout.Label("PresetName:", EditorStyles.boldLabel);
            item.PresetName = EditorGUILayout.TextField("", item.PresetName);
            GUILayout.BeginVertical();
            GUILayout.Label("LongName:", EditorStyles.boldLabel);
            item.LongName = EditorGUILayout.TextField("", item.LongName);
            GUILayout.Label("ShortName:", EditorStyles.boldLabel);
            item.ShortName = EditorGUILayout.TextField("", item.ShortName);
            if (GUILayout.Button("Remove", GUILayout.Width(70)))
            {
                mConfigItems.ItemList.Remove(item);
                remove = true;
            }

            GUILayout.EndVertical();
            GUILayout.EndHorizontal();

            return item;
        }

        private void OnGUI()
        {
            mScrollPosition = GUILayout.BeginScrollView(mScrollPosition, GUILayout.Width(500), GUILayout.Height(500));
            var count = mConfigItems.ItemList.Count;
            for (int i = 0; i < count; ++i)
            {
                bool remove = false;
                var item = OnItemGUI(mConfigItems.ItemList[i], ref remove);
                if (remove)
                {
                    --count;
                }
                else
                {
                    mConfigItems.ItemList[i] = item;
                }
            }
            GUILayout.EndScrollView();

            GUILayout.FlexibleSpace();
            if (GUILayout.Button("Add", GUILayout.Width(50), GUILayout.Height(25)))
            {
                mConfigItems.ItemList.Add(new Texture.PresetConfigItem());
            }
        }

        private void OnEnable()
        {
            var configParser = new Texture.ConfigParser();
            mConfigItems = configParser.Load();
            if (mConfigItems.ItemList == null)
            {
                mConfigItems.ItemList = new List<Texture.PresetConfigItem>();
            }
        }

        private void OnDestroy()
        {
            var configParser = new Texture.ConfigParser();
            configParser.Save(ref mConfigItems);
        }
    }
}
