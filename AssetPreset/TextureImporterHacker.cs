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
            public Preset Preset;

            public Texture.PresetConfigItem ConfigItem;
        }

        static bool mInited = false;
        static List<TexturePreset> mTexturePresets;
        static System.DateTime mFileTime;

        private TexturePreset GetTexturePreset(string path)
        {
            for (int i = 0; i < mTexturePresets.Count; ++i)
            {
                for (int j = 0; j < mTexturePresets[i].ConfigItem.NamingConventions.Length; ++j)
                {
                    var namingConvention = mTexturePresets[i].ConfigItem.NamingConventions[j];
                    if (path.Contains(namingConvention) || path.Contains(namingConvention.ToLower()))
                    {
                        return mTexturePresets[i];
                    }
                }
            }

            return new TexturePreset();
        }

        public void Init()
        {
            if (mInited)
            {
                return;
            }

            var config = new Texture.ConfigParser();
            mFileTime = config.GetFileTime();

            var configItems = config.Load();
            if (configItems.Items == null)
            {
                return;
            }

            List<Texture.PresetConfigItem> items = new List<Texture.PresetConfigItem>();
            for (int i = 0; i < configItems.Items.Length; ++i)
            {
                if (configItems.Items[i].PresetName == null)
                {
                    continue;
                }

                int validItems = 0;
                for (int j = 0; j < configItems.Items[i].NamingConventions.Length; ++j)
                {
                    if (configItems.Items[i].NamingConventions[j] != null &&
                        configItems.Items[i].NamingConventions[j].Length > 0)
                    {
                        ++validItems;
                    }
                }

                if (validItems > 0)
                {
                    var item = new Texture.PresetConfigItem();
                    item.PresetName = configItems.Items[i].PresetName;
                    item.NamingConventions = new string[validItems];

                    for (int j = 0; j < configItems.Items[i].NamingConventions.Length; ++j)
                    {
                        if (configItems.Items[i].NamingConventions[j] != null &&
                            configItems.Items[i].NamingConventions[j].Length > 0)
                        {
                            item.NamingConventions[j] = configItems.Items[i].NamingConventions[j];
                        }
                    }
                    items.Add(item);
                }
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
                        preset.Preset = (Preset)AssetDatabase.LoadAssetAtPath(path, typeof(Preset));
                        mTexturePresets.Add(preset);
                    }
                }
            }

            mInited = true;
        }

        public void OnPreprocessTexture()
        {
            ///return;
            var config = new Texture.ConfigParser();
            var fileTime = config.GetFileTime();
            if (fileTime != mFileTime)
            {
                mInited = false;
            }

            Init();

            var preset = GetTexturePreset(assetPath);

            #region HardCode
            var textureImporter = (TextureImporter)assetImporter;
            textureImporter.sRGBTexture = false;
            var androidTextureSettings = textureImporter.GetPlatformTextureSettings("Android");
            androidTextureSettings.overridden = true;
            androidTextureSettings.compressionQuality = 100;
            androidTextureSettings.resizeAlgorithm = TextureResizeAlgorithm.Mitchell;

            if (preset.ConfigItem.PresetName == "Texture_AlbedoTransparency")
            {
                androidTextureSettings.format = TextureImporterFormat.ASTC_5x5;
                androidTextureSettings.maxTextureSize = 2048;
                textureImporter.sRGBTexture = true;
            }
            else if (preset.ConfigItem.PresetName == "Texture_Normal")
            {
                androidTextureSettings.format = TextureImporterFormat.ASTC_4x4;
                androidTextureSettings.maxTextureSize = 2048;
                textureImporter.textureType = TextureImporterType.NormalMap;
            }
            else if (preset.ConfigItem.PresetName == "Texture_OcclusionMetallic")
            {
                androidTextureSettings.format = TextureImporterFormat.ASTC_8x8;
                androidTextureSettings.maxTextureSize = 1024;
            }
            else if (preset.ConfigItem.PresetName == "Texture_Glossiness")
            {
                androidTextureSettings.format = TextureImporterFormat.ASTC_6x6;
                androidTextureSettings.maxTextureSize = 2048;
            }
            else if (preset.ConfigItem.PresetName == "Texture_AO")
            {
                androidTextureSettings.format = TextureImporterFormat.ASTC_8x8;
                androidTextureSettings.maxTextureSize = 1024;
            }
            else if (preset.ConfigItem.PresetName == "Texture_MetallicSmoothness")
            {
                androidTextureSettings.format = TextureImporterFormat.ASTC_6x6;
                androidTextureSettings.maxTextureSize = 1024;
            }
            else if (preset.ConfigItem.PresetName == "Texture_Emission")
            {
                androidTextureSettings.format = TextureImporterFormat.R8;
                androidTextureSettings.maxTextureSize = 512;
            }
            else
            {
                return;
            }
            textureImporter.SetPlatformTextureSettings(androidTextureSettings);
            #endregion

            //if (preset.Preset != null)
            //{
            //    var tempAssetBundleName = string.Copy(assetImporter.assetBundleName);
            //    var tempAssetBundleVariant = string.Copy(assetImporter.assetBundleVariant);
            //    ///var textureImporter = (TextureImporter)assetImporter;
            //    if (!preset.Preset.ApplyTo(assetImporter))
            //    {
            //        Debug.LogError(string.Format("Faild to apply preset to texture {0}", assetPath));
            //    }
            //    assetImporter.SetAssetBundleNameAndVariant(tempAssetBundleName, tempAssetBundleVariant);
            //}
        }
    }
}
