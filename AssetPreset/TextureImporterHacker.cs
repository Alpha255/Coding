using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using UnityEditor.Presets;
using System.IO;

namespace BlackJack.Tat.Tools.Unity.AssetPreset
{
    public class TextureProcessor : AssetPostprocessor
    {
        struct TexturePreset
        {
            public Preset PresetObject;

            public Texture.PresetConfigItem ConfigItem;
        }

        static bool mInited = false;
        static List<TexturePreset> mTexturePresets;
        static System.DateTime mConfigFileTime;

        private Preset GetTexturePreset(string texturePath)
        {
            for (int i = 0; i < mTexturePresets.Count; ++i)
            {
                if (texturePath.Contains(mTexturePresets[i].ConfigItem.LongName) ||
                    texturePath.Contains(mTexturePresets[i].ConfigItem.LongName.ToLower()) ||
                    texturePath.EndsWith(mTexturePresets[i].ConfigItem.ShortName))
                {
                    return mTexturePresets[i].PresetObject;
                }
            }

            return null;
        }

        public void Init()
        {
            if (mInited)
            {
                return;
            }

            var config = new Texture.ConfigParser();
            mConfigFileTime = config.GetConfigFileTime();

            var configItems = config.Load();
            if (configItems.ItemList == null || configItems.ItemList.Count == 0)
            {
                return;
            }

            List<Texture.PresetConfigItem> items = new List<Texture.PresetConfigItem>();
            for (int i = 0; i < configItems.ItemList.Count; ++i)
            {
                if (!configItems.ItemList[i].IsValid())
                {
                    continue;
                }

                var item = new Texture.PresetConfigItem();
                item.PresetName = configItems.ItemList[i].PresetName;
                item.LongName = configItems.ItemList[i].LongName;
                item.ShortName = configItems.ItemList[i].ShortName;
                items.Add(item);
            }

            mTexturePresets = new List<TexturePreset>();
            var presetGUIDs = AssetDatabase.FindAssets("t:preset");
            for (int i = 0; i < presetGUIDs.Length; ++i)
            {
                for (int j = 0; j < items.Count; ++j)
                {
                    var path = AssetDatabase.GUIDToAssetPath(presetGUIDs[i]);
                    if (path.Contains(items[j].PresetName))
                    {
                        var preset = new TexturePreset();
                        preset.ConfigItem = items[j];
                        preset.PresetObject = (Preset)AssetDatabase.LoadAssetAtPath(path, typeof(Preset));
                        mTexturePresets.Add(preset);
                    }
                }
            }

            mInited = true;
        }

        public void OnPreprocessTexture()
        {
            var config = new Texture.ConfigParser();
            var fileTime = config.GetConfigFileTime();
            if (fileTime != mConfigFileTime)
            {
                mInited = false;
            }

            Init();

            var preset = GetTexturePreset(assetPath);

            #region HardCode
            //var textureImporter = (TextureImporter)assetImporter;
            //textureImporter.sRGBTexture = false;
            //var androidTextureSettings = textureImporter.GetPlatformTextureSettings("Android");
            //androidTextureSettings.overridden = true;
            //androidTextureSettings.compressionQuality = 100;
            //androidTextureSettings.resizeAlgorithm = TextureResizeAlgorithm.Mitchell;

            //if (preset.ConfigItem.PresetName == "Texture_AlbedoTransparency")
            //{
            //    androidTextureSettings.format = TextureImporterFormat.ASTC_5x5;
            //    androidTextureSettings.maxTextureSize = 2048;
            //    textureImporter.sRGBTexture = true;
            //}
            //else if (preset.ConfigItem.PresetName == "Texture_Normal")
            //{
            //    androidTextureSettings.format = TextureImporterFormat.ASTC_4x4;
            //    androidTextureSettings.maxTextureSize = 2048;
            //    textureImporter.textureType = TextureImporterType.NormalMap;
            //}
            //else if (preset.ConfigItem.PresetName == "Texture_OcclusionMetallic")
            //{
            //    androidTextureSettings.format = TextureImporterFormat.ASTC_8x8;
            //    androidTextureSettings.maxTextureSize = 1024;
            //}
            //else if (preset.ConfigItem.PresetName == "Texture_Glossiness")
            //{
            //    androidTextureSettings.format = TextureImporterFormat.ASTC_6x6;
            //    androidTextureSettings.maxTextureSize = 2048;
            //}
            //else if (preset.ConfigItem.PresetName == "Texture_AO")
            //{
            //    androidTextureSettings.format = TextureImporterFormat.ASTC_8x8;
            //    androidTextureSettings.maxTextureSize = 1024;
            //}
            //else if (preset.ConfigItem.PresetName == "Texture_MetallicSmoothness")
            //{
            //    androidTextureSettings.format = TextureImporterFormat.ASTC_6x6;
            //    androidTextureSettings.maxTextureSize = 1024;
            //}
            //else if (preset.ConfigItem.PresetName == "Texture_Emission")
            //{
            //    androidTextureSettings.format = TextureImporterFormat.R8;
            //    androidTextureSettings.maxTextureSize = 512;
            //}
            //else
            //{
            //    return;
            //}
            //textureImporter.SetPlatformTextureSettings(androidTextureSettings);
            #endregion

            if (preset != null)
            {
                var textureImporter = (TextureImporter)assetImporter;
                List<string> targetProperties = new List<string>();
                for (int i = 0; i < preset.PropertyModifications.Length; ++i)
                {
                    if (preset.PropertyModifications[i].propertyPath == "m_AssetBundleName" ||
                        preset.PropertyModifications[i].propertyPath == "m_AssetBundleVariant")
                    {
                        continue;
                    }
                    targetProperties.Add(preset.PropertyModifications[i].propertyPath);
                }
                if (!preset.ApplyTo(textureImporter, targetProperties.ToArray()))
                {
                    Debug.LogError(string.Format("Faild to apply preset to texture {0}", assetPath));
                }
            }
        }
    }
}
