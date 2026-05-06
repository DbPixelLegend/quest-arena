import unreal
unreal.log('HAS_BlueprintEditorLibrary=' + str(hasattr(unreal, 'BlueprintEditorLibrary')))
unreal.log('HAS_KismetEditorUtilities=' + str(hasattr(unreal, 'KismetEditorUtilities')))
unreal.log('HAS_EditorAssetLibrary=' + str(hasattr(unreal, 'EditorAssetLibrary')))
if hasattr(unreal, 'EditorAssetLibrary'):
    unreal.log('EAL_OK')
unreal.SystemLibrary.quit_editor()
