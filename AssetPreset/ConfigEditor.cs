using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

namespace BlackJack.Tat.Tools.Unity.AssetPreset
{
    public class ConfigEditor : EditorWindow
    {
        private List<Texture.PresetConfigItem> mConfigItems = new List<Texture.PresetConfigItem>();
        private Vector2 mScrollPosition;

        [MenuItem("Assets/BlackJack/AssetPresetConfig")]
        static public void ShowWindow()
        {
            EditorWindow.GetWindow<ConfigEditor>("Asset Preset Configuration").Show();
        }

        private Texture.PresetConfigItem OnItemGUI(Texture.PresetConfigItem item)
        {
            GUILayout.BeginHorizontal("Box");
            GUILayout.Label("Preset", EditorStyles.boldLabel);
            item.PresetName = EditorGUILayout.TextField("", item.PresetName);
            GUILayout.BeginVertical();
            GUILayout.Label("TextureNamingConventions", EditorStyles.boldLabel);
            for (int i = 0; i < item.NamingConventions.Length; ++i)
            {
                item.NamingConventions[i] = EditorGUILayout.TextField("", item.NamingConventions[i]);
            }
            if (GUILayout.Button("Remove", GUILayout.Width(70)))
            {
                mConfigItems.Remove(item);
            }

            GUILayout.EndVertical();
            GUILayout.EndHorizontal();

            return item;
        }

        private void AddItem()
        {
            var item = new Texture.PresetConfigItem();
            item.NamingConventions = new string[2];
            mConfigItems.Add(item);
        }

        private void OnGUI()
        {
            mScrollPosition = GUILayout.BeginScrollView(mScrollPosition, GUILayout.Width(500), GUILayout.Height(500));
            for (int i = 0; i < mConfigItems.Count; ++i)
            {
                mConfigItems[i] = OnItemGUI(mConfigItems[i]);
            }
            GUILayout.EndScrollView();

            GUILayout.FlexibleSpace();
            if (GUILayout.Button("Add", GUILayout.Width(50), GUILayout.Height(25)))
            {
                AddItem();
            }
        }

        private void OnEnable()
        {
            var configParser = new Texture.ConfigParser();
            var items = configParser.Load();
            if (items.Items != null)
            {
                for (int i = 0; i < items.Items.Length; ++i)
                {
                    mConfigItems.Add(items.Items[i]);
                }
            }
        }

        private void OnDestroy()
        {
            var configParser = new Texture.ConfigParser();
            configParser.Save(ref mConfigItems);
        }
    }
}
