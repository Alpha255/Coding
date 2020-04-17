using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using System.IO;

namespace BlackJack.Tat.Tools.Unity.AssetPreset
{
    namespace Texture
    {
        [System.Serializable]
        public struct PresetConfigItem
        {
            public string PresetName;          
            public string[] NamingConventions;
        }

        [System.Serializable]
        public struct PresetConfigItems
        {
            public PresetConfigItem[] Items;
        }

        public class ConfigParser
        {
            private bool mDirty = true;
            public bool Dirty
            {
                get { return mDirty; }
                set { mDirty = value; }
            }

            private string mJsonName = "JsonConfig_Texture";

            private string GetJsonPath()
            {
                var findResult = AssetDatabase.FindAssets(mJsonName);
                if (findResult != null)
                {
                    return AssetDatabase.GUIDToAssetPath(findResult[0]);
                }

                return null;
            }

            public System.DateTime GetFileTime()
            {
                var jsonPath = GetJsonPath();
                if (jsonPath != null)
                {
                    return File.GetLastAccessTime(jsonPath);
                }

                return new System.DateTime();
            }

            private bool IsValid(ref List<PresetConfigItem> items)
            {
                for (int i = 0; i < items.Count; ++i)
                {
                    if (items[i].PresetName != null)
                    {
                        for (int j = 0; j < items[i].NamingConventions.Length; ++j)
                        {
                            if (items[i].NamingConventions[j] != null)
                            {
                                return true;
                            }
                        }
                    }
                }

                return false;
            }

            public PresetConfigItems Load()
            {
                var jsonPath = GetJsonPath();
                if (jsonPath != null)
                {
                    string jsonData = File.ReadAllText(jsonPath);
                    if (jsonData != null && jsonData.Length > 0)
                    {
                        return JsonUtility.FromJson<PresetConfigItems>(jsonData);
                    }
                }
                else
                {
                    Debug.LogError("Load::Can't find texture preset configuration file.");
                }

                return new PresetConfigItems();
            }

            public void Save(ref List<PresetConfigItem> items)
            {
                var jsonPath = GetJsonPath();
                if (jsonPath != null)
                {
                    if (!IsValid(ref items))
                    {
                        return;
                    }
                    var configItems = new PresetConfigItems();
                    configItems.Items = new PresetConfigItem[items.Count];
                    for (int i = 0; i < items.Count; ++i)
                    {
                        configItems.Items[i] = items[i];
                    }

                    string jsonData = JsonUtility.ToJson(configItems, true);
                    File.WriteAllText(jsonPath, jsonData);
                }
                else
                {
                    Debug.LogError("Save::Can't find texture preset configuration file.");
                }
            }
        }
    }
}
