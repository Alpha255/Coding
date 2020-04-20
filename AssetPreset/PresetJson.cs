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
            public string LongName;
            public string ShortName;

            public bool IsValid()
            {
                return (PresetName != null && PresetName.Length > 0) &&
                    (LongName != null && LongName.Length > 0) &&
                    (ShortName != null && ShortName.Length > 0);
            }
        }

        [System.Serializable]
        public struct PresetConfigItems
        {
            public List<PresetConfigItem> ItemList;
        }

        public class ConfigParser
        {
            private string mConfigName = "JsonConfig_Texture";

            private string GetConfigPath()
            {
                var findResult = AssetDatabase.FindAssets(mConfigName);
                if (findResult != null)
                {
                    for (int i = 0; i < findResult.Length; ++i)
                    {
                        var path = AssetDatabase.GUIDToAssetPath(findResult[i]);
                        if (path.Contains("\\AssetPreset\\" + mConfigName + ".json") ||
                            path.Contains("/AssetPreset/" + mConfigName + ".json"))
                        {
                            return path;
                        }
                    }
                }

                return null;
            }

            public System.DateTime GetConfigFileTime()
            {
                var path = GetConfigPath();
                if (path != null)
                {
                    return File.GetLastAccessTime(path);
                }

                return new System.DateTime();
            }

            public PresetConfigItems Load()
            {
                var path = GetConfigPath();
                if (path != null)
                {
                    string jsonData = File.ReadAllText(path);
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

            public void Save(ref PresetConfigItems items)
            {
                var path = GetConfigPath();
                if (path != null && 
                    items.ItemList != null && 
                    items.ItemList.Count > 0)
                {
                    var configItems = new PresetConfigItems();
                    configItems.ItemList = new List<PresetConfigItem>();
                    for (int i = 0; i < items.ItemList.Count; ++i)
                    {
                        if (items.ItemList[i].IsValid())
                        {
                            configItems.ItemList.Add(items.ItemList[i]);
                        }
                    }

                    if (configItems.ItemList.Count > 0)
                    {
                        string jsonData = JsonUtility.ToJson(configItems, true);
                        File.WriteAllText(path, jsonData);
                    }
                }
            }
        }
    }
}
