class ToolchainConfig {
  const ToolchainConfig({
    required this.blenderExePath,
    required this.fast64InstallPath,
    required this.fast64PinnedTag,
    required this.fast64ProjectRoot,
    this.pythonPath,
  });

  final String blenderExePath;
  final String fast64InstallPath;
  final String fast64PinnedTag;
  final String fast64ProjectRoot;
  final String? pythonPath;

  bool get hasConfiguredBlender => blenderExePath.trim().isNotEmpty;
  bool get hasConfiguredFast64 => fast64InstallPath.trim().isNotEmpty;
  bool get hasConfiguredProjectRoot => fast64ProjectRoot.trim().isNotEmpty;

  bool get isConfigured =>
      hasConfiguredBlender &&
      hasConfiguredFast64 &&
      hasConfiguredProjectRoot &&
      fast64PinnedTag.trim().isNotEmpty;

  ToolchainConfig copyWith({
    String? blenderExePath,
    String? fast64InstallPath,
    String? fast64PinnedTag,
    String? fast64ProjectRoot,
    String? pythonPath,
    bool clearPythonPath = false,
  }) {
    return ToolchainConfig(
      blenderExePath: blenderExePath ?? this.blenderExePath,
      fast64InstallPath: fast64InstallPath ?? this.fast64InstallPath,
      fast64PinnedTag: fast64PinnedTag ?? this.fast64PinnedTag,
      fast64ProjectRoot: fast64ProjectRoot ?? this.fast64ProjectRoot,
      pythonPath: clearPythonPath ? null : (pythonPath ?? this.pythonPath),
    );
  }

  Map<String, dynamic> toJson() => {
    "blenderExePath": blenderExePath,
    "fast64InstallPath": fast64InstallPath,
    "fast64PinnedTag": fast64PinnedTag,
    "fast64ProjectRoot": fast64ProjectRoot,
    "pythonPath": pythonPath,
  };

  factory ToolchainConfig.fromJson(
    Map<String, dynamic> json, {
    required String defaultPinnedTag,
  }) {
    String readString(String key) {
      final value = json[key];
      return value is String ? value : "";
    }

    final rawPinnedTag = readString("fast64PinnedTag").trim();

    return ToolchainConfig(
      blenderExePath: readString("blenderExePath"),
      fast64InstallPath: readString("fast64InstallPath"),
      fast64PinnedTag: rawPinnedTag.isEmpty ? defaultPinnedTag : rawPinnedTag,
      fast64ProjectRoot: readString("fast64ProjectRoot"),
      pythonPath: json["pythonPath"] is String
          ? json["pythonPath"] as String
          : null,
    );
  }

  factory ToolchainConfig.empty({required String defaultPinnedTag}) {
    return ToolchainConfig(
      blenderExePath: "",
      fast64InstallPath: "",
      fast64PinnedTag: defaultPinnedTag,
      fast64ProjectRoot: "",
      pythonPath: null,
    );
  }
}
