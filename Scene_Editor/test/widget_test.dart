import "dart:ui";

import "package:flutter_test/flutter_test.dart";

import "package:scene_editor/main.dart";

void main() {
  testWidgets("Scene editor smoke test", (tester) async {
    tester.view.physicalSize = const Size(1920, 1080);
    tester.view.devicePixelRatio = 1.0;

    addTearDown(() {
      tester.view.resetPhysicalSize();
      tester.view.resetDevicePixelRatio();
    });

    await tester.pumpWidget(const SceneEditorApp());

    expect(find.text("Scene Editor MVP"), findsOneWidget);
    expect(find.text("Load ROM"), findsOneWidget);
    expect(find.text("Scenes"), findsOneWidget);
  });
}
