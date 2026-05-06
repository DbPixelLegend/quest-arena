import unreal
has_bel = hasattr(unreal, 'BlueprintEditorLibrary')
has_keu = hasattr(unreal, 'KismetEditorUtilities')
has_eal = hasattr(unreal, 'EditorAssetLibrary')
unreal.log('HAS_BlueprintEditorLibrary=' + str(has_bel))
unreal.log('HAS_KismetEditorUtilities=' + str(has_keu))
unreal.log('HAS_EditorAssetLibrary=' + str(has_eal))
if has_bel:
    methods=[n for n in dir(unreal.BlueprintEditorLibrary) if ('graph' in n.lower() or 'node' in n.lower() or 'find' in n.lower())]
    unreal.log('BEL_METHODS=' + ','.join(methods[:80]))
if has_keu:
    methods=[n for n in dir(unreal.KismetEditorUtilities) if ('blueprint' in n.lower() or 'graph' in n.lower() or 'compile' in n.lower() or 'component' in n.lower())]
    unreal.log('KEU_METHODS=' + ','.join(methods[:120]))
