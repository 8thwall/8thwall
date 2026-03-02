using UnityEngine;
using UnityEditor;
using UnityEditor.Android;
using UnityEditor.Callbacks;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using UnityEditor.iOS.Xcode;
using UnityEditor.Build;
using UnityEditor.Build.Reporting;

class MyCustomBuildProcessor : IPreprocessBuildWithReport
{
    public int callbackOrder { get { return 0; } }
    public void OnPreprocessBuild(BuildReport report)
    {
      BazelBuild.ParseCommandLineArguments();
      AndroidExternalToolsSettings.gradlePath = BazelBuild.DetermineGradlePath();
      // Make gradle's maxJvmHeapSize 16gb so that we don't have to worry about
      // running out of memory on larger builds
      AndroidExternalToolsSettings.maxJvmHeapSize = 16000;
    }
}

// Unity will try to Codesign Apple dylibs coming from Bazel, but they will
// fail due to being unwritable. This will add chmod u+x to allow the build to
// succeed.
#if UNITY_EDITOR_OSX
class OSXBundleChmodPostprocessor : AssetPostprocessor
{
  static private Regex osxBundleRegex = new Regex(@"^Assets/Plugins/macOS/.*\.bundle$", RegexOptions.Compiled);
  static void OnPostprocessAllAssets(string[] importedAssets, string[] deletedAssets, string[] movedAssets, string[] movedFromAssetPaths, bool didDomainReload)
  {
    foreach (string str in importedAssets)
    {
      MatchCollection matches = osxBundleRegex.Matches(str);
      if (matches.Count > 0) {
        // Current version of Mono and Unity doesn't have support for
        // Mono.Unix.UnixFileInfo, nor the ability to update BSD Unix file
        // permissions with C# commands like File.SetAttributes.
        // Here we use 'chmod' in a shell.
        try
        {
          using (System.Diagnostics.Process proc = System.Diagnostics.Process.Start("/bin/bash", $"-c \"chmod u+w \\\"{str}\\\"\""))
          {
            proc.WaitForExit();
            if (proc.ExitCode != 0) {
              Debug.LogError(String.Format("Failed to update write permissions for {0}", str));
            }
          }
        }
        catch
        {
          Debug.LogError(String.Format("Exception when updating write permissions for {0}", str));
        }
      }
    }
  }
}
#endif  // UNITY_EDITOR_OSX

public static class BazelBuild {
  static string SCENES_TAG = "--scenes";
  static string GRADLE_TAG = "--gradle";
  static string PROVISIONING_PROFILE_TAG = "--provisioningProfile";
  static string DISTRIBUTION_METHOD_TAG = "--distributionMethod";
  static string APP_NAME_TAG = "--appname";
  static string CONFIG_TAG = "--config";

  static Dictionary<string, string> parsedCommandLineArgs = new Dictionary<string, string>()
  {
    { SCENES_TAG, string.Empty },
    { GRADLE_TAG, string.Empty },
    { PROVISIONING_PROFILE_TAG, string.Empty },
    { DISTRIBUTION_METHOD_TAG, string.Empty },
    { APP_NAME_TAG, string.Empty },
    { CONFIG_TAG, string.Empty },
  };

  public static void ParseCommandLineArguments() {
    string[] commandLineArgs = Environment.GetCommandLineArgs();

    // attempt to parse command line arguments and record them parsedCommandLineArgs dictionary
    for (var i = 0; i < commandLineArgs.Length-1; i++) {
      string commandLineKey = commandLineArgs[i];
      if (parsedCommandLineArgs.ContainsKey(commandLineKey)) {
        parsedCommandLineArgs[commandLineKey] = commandLineArgs[i+1];
      }
    }
  }

  public static string[] DetermineIncludedScenes(string[] commandLineArgs) {
    var defaultValue = new List<string>();
    defaultValue.AddRange(EditorBuildSettings.scenes.Select(scene => scene.path));
    var argumentValue = new List<string>();
    foreach (var scene in parsedCommandLineArgs[SCENES_TAG].Split(',')) {
      // ensure that the scene is indeed a valid scene that ends with ".unity"
      if (scene.Split('.').Last() == "unity") {
        argumentValue.Add(scene);
      }
    }
    return argumentValue.Count > 0 ? argumentValue.ToArray() : defaultValue.ToArray();
  }

  public static string DetermineGradlePath() {
    var defaultValue = AndroidExternalToolsSettings.gradlePath;
    string argumentValue = parsedCommandLineArgs[GRADLE_TAG];
    return argumentValue != string.Empty ? argumentValue : defaultValue;
  }

  public static string DetermineProvisioningProfile() {
    var defaultValue = PlayerSettings.iOS.iOSManualProvisioningProfileID;
    string argumentValue = parsedCommandLineArgs[PROVISIONING_PROFILE_TAG];
    return argumentValue != string.Empty ? argumentValue : defaultValue;
  }

  public static string DetermineDistributionMethod() {
    var defaultValue = "development";
    string argumentValue = parsedCommandLineArgs[DISTRIBUTION_METHOD_TAG];
    return argumentValue != string.Empty ? argumentValue : defaultValue;
  }

  public static string DetermineProductName() {
    var defaultValue = PlayerSettings.productName;
    string argumentValue = parsedCommandLineArgs[APP_NAME_TAG];
    return argumentValue != string.Empty ? argumentValue : defaultValue;
  }

  public static string DetermineBuildConfig() {
    var defaultValue = "release";
    string argumentValue = parsedCommandLineArgs[CONFIG_TAG];
    return argumentValue != string.Empty ? argumentValue : defaultValue;
  }

  static void BuildCommon(BuildTargetGroup buildTargetGroup, BuildTarget buildTarget) {
    ParseCommandLineArguments();
    EditorUserBuildSettings.SwitchActiveBuildTarget(buildTargetGroup, buildTarget);
    PlayerSettings.companyName = "nianticlabs";
    PlayerSettings.productName = DetermineProductName();
    string identifier = $"com.{PlayerSettings.companyName}.{PlayerSettings.productName}";
    PlayerSettings.SetApplicationIdentifier(buildTargetGroup, identifier);
  }

  // Build xcode project for iOS builds.
  [MenuItem("BazelBuild/iOS")]
  static void BuildiOS () {
    BuildCommon(BuildTargetGroup.iOS, BuildTarget.iOS);

    string outputDir = {xcodeOutputDir};
    string iosSdkPlatform = {iosSdkPlatform};
    Directory.CreateDirectory(outputDir);

    // TODO(mc): Make build options conditional on debug/release builds.
#if UNITY_2021_2_OR_NEWER
    BuildOptions options = BuildOptions.SymlinkSources;
    EditorUserBuildSettings.symlinkSources = true;
#else
    BuildOptions options = BuildOptions.SymlinkLibraries;
    EditorUserBuildSettings.symlinkLibraries = true;
#endif

    if (DetermineBuildConfig() == "debug")
    {
      options = options | BuildOptions.ConnectWithProfiler;
    }

    // Set minimum OS version.
    if (Version.Parse(PlayerSettings.iOS.targetOSVersionString) < Version.Parse({iosMinimumVersion}))
    {
      PlayerSettings.iOS.targetOSVersionString = {iosMinimumVersion};
    }

    if (iosSdkPlatform == "iPhoneSimulator") {
      PlayerSettings.iOS.sdkVersion = iOSSdkVersion.SimulatorSDK;
    } else {
      // Else: iosSdkPlatform == "iPhoneOS".
      PlayerSettings.iOS.sdkVersion = iOSSdkVersion.DeviceSDK;
    }

    BuildPipeline.BuildPlayer(
      DetermineIncludedScenes(Environment.GetCommandLineArgs()),
      outputDir,
      BuildTarget.iOS,
      options);
  }

  // Build Android Project.
  [MenuItem("BazelBuild/Android")]
  static void BuildAndroid () {
    BuildCommon(BuildTargetGroup.Android, BuildTarget.Android);
    BuildPipeline.BuildPlayer(
      DetermineIncludedScenes(Environment.GetCommandLineArgs()),
      {androidOutput},
      BuildTarget.Android,
      BuildOptions.None);
  }

  // Build OSX Project
  [MenuItem("BazelBuild/OSX")]
  static void BuildOSX () {
    BuildCommon(BuildTargetGroup.Standalone, BuildTarget.StandaloneOSX);
    BuildPipeline.BuildPlayer(
      DetermineIncludedScenes(Environment.GetCommandLineArgs()),
      {osxOutput},
      BuildTarget.StandaloneOSX,
      BuildOptions.None);
  }

  [PostProcessBuild]
  public static void XcodeProjectSettings(BuildTarget buildTarget, string pathToBuiltProject) {
    if (buildTarget == BuildTarget.iOS) {
      string projPath = pathToBuiltProject + "/Unity-iPhone.xcodeproj/project.pbxproj";
      PBXProject proj = new PBXProject();
      proj.ReadFromFile(projPath);
      #if UNITY_2019_3_OR_NEWER
      string unityTarget = proj.GetUnityMainTargetGuid();
      #else
      string unityTarget = proj.TargetGuidByName("Unity-iPhone");
      #endif

      string bitcodeSetting = {bitcodeSetting};

      proj.SetBuildProperty(unityTarget, "ENABLE_BITCODE", bitcodeSetting);

      string unityFrameworkTarget = proj.GetUnityFrameworkTargetGuid();
      proj.SetBuildProperty(unityFrameworkTarget, "ENABLE_BITCODE", bitcodeSetting);

      string unityTestTarget = proj.TargetGuidByName(PBXProject.GetUnityTestTargetName());
      proj.SetBuildProperty(unityTestTarget, "ENABLE_BITCODE", bitcodeSetting);

      // Set the team ID (should always be this as all our apps are under niantic team)
      proj.SetBuildProperty(unityTarget, "DEVELOPMENT_TEAM", "<REMOVED_BEFORE_OPEN_SOURCING>");

      // Set the signing to manual as ci runners don't have provisioning profiles installed for each app
      proj.SetBuildProperty(unityTarget, "CODE_SIGN_STYLE", "Manual");

      // Set the certificate type based on developemnt / adhoc / appstore builds
      string distributionMethod = DetermineDistributionMethod();
      string certificateType = (distributionMethod == "ad-hoc" || distributionMethod == "app-store") ? "Apple Distribution" : "Apple Development";
      proj.SetBuildProperty(unityTarget, "CODE_SIGN_IDENTITY", certificateType);

      // Set the profile ID to be fetched from apple from profiles under the team id (provided by CLI)
      string profile = DetermineProvisioningProfile();
      if (profile == string.Empty) {
        profile = "Wildcard Development";
      }
      proj.SetBuildProperty(unityTarget, "PROVISIONING_PROFILE_SPECIFIER", profile);

      // TODO(mc): Disable warnings that fire spuriously in XCode builds.
      proj.SetBuildProperty(unityTarget, "GCC_INLINES_ARE_PRIVATE_EXTERN", "NO");
      proj.SetBuildProperty(unityTarget, "GCC_SYMBOLS_PRIVATE_EXTERN", "NO");
      proj.SetBuildProperty(unityTarget, "GCC_WARN_UNUSED_VARIABLE", "NO");
      proj.UpdateBuildProperty(unityTarget, "WARNING_CFLAGS", new string[]{
          "-Wno-extern-initializer",
          "-Wno-deprecated-declarations",
          "-Wno-return-type",
          "-Wno-deprecated-implementations"}, new string[]{});

      // Write back the project file.
      proj.WriteToFile(projPath);
    }
  }
}
