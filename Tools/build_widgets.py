# Copyright (c) 2026 The Last Cartographer.
#
# Build real UMG layouts via Python. Called by bootstrap.py after the
# widget Blueprint stubs already exist. We add child widgets to each
# WBP_*'s widget tree using the names the C++ NativeTick code expects
# (BindWidgetOptional binding by name).
#
# All API calls are wrapped in try/except — UE5 minor versions move
# things around. Falling back to "asset exists, layout empty" is the
# safe failure mode (the C++ won't null-deref because everything uses
# BindWidgetOptional).

import unreal


EAS = unreal.EditorAssetLibrary
ALT = unreal.AssetToolsHelpers.get_asset_tools()


def log(msg):  unreal.log("[widgets] " + msg)
def warn(msg): unreal.log_warning("[widgets] " + msg)


def get_tree(wbp):
    try:
        return wbp.get_editor_property("widget_tree")
    except Exception as e:
        warn("widget_tree access failed: " + str(e))
        return None


def construct(tree, cls, name):
    """tree.construct_widget exists in UE 5.x via WidgetTree."""
    try:
        return tree.construct_widget(cls, name)
    except Exception as e:
        warn("construct_widget({}, {}) failed: {}".format(cls.__name__, str(e)))
        return None


def make_fill_size(value=1.0):
    s = unreal.SlateChildSize()
    try:
        s.set_editor_property("value", value)
        s.set_editor_property("size_rule", unreal.SlateSizeRule.FILL)
    except Exception:
        try:
            s.value = value
            s.size_rule = unreal.SlateSizeRule.FILL
        except Exception:
            pass
    return s


def set_root(tree, root):
    try:
        tree.set_editor_property("root_widget", root)
    except Exception as e:
        warn("set root_widget failed: " + str(e))


def make_anchors(min_x, min_y, max_x, max_y):
    a = unreal.Anchors()
    try:
        a.set_editor_property("minimum", unreal.Vector2D(min_x, min_y))
        a.set_editor_property("maximum", unreal.Vector2D(max_x, max_y))
    except Exception:
        try:
            a.minimum = unreal.Vector2D(min_x, min_y)
            a.maximum = unreal.Vector2D(max_x, max_y)
        except Exception:
            pass
    return a


def add_to_canvas(canvas, widget, anchors=(0,0,1,1), offsets=(0,0,0,0), alignment=(0,0)):
    """Add a child to a Canvas Panel with specific anchor + offset."""
    if canvas is None or widget is None: return None
    try:
        slot = canvas.add_child_to_canvas(widget)
        slot.set_editor_property("anchors", make_anchors(*anchors))
        slot.set_editor_property("offsets", unreal.Margin(*offsets))
        slot.set_editor_property("alignment", unreal.Vector2D(*alignment))
        slot.set_editor_property("auto_size", False)
        return slot
    except Exception as e:
        warn("canvas slot setup failed: " + str(e))
        return None


def vbox_add(box, widget, padding=(4,4,4,4)):
    if box is None or widget is None: return None
    try:
        slot = box.add_child_to_vertical_box(widget)
        slot.set_editor_property("padding", unreal.Margin(*padding))
        return slot
    except Exception as e:
        warn("vbox add failed: " + str(e))
        return None


def hbox_add(box, widget, padding=(4,4,4,4)):
    if box is None or widget is None: return None
    try:
        slot = box.add_child_to_horizontal_box(widget)
        slot.set_editor_property("padding", unreal.Margin(*padding))
        return slot
    except Exception as e:
        warn("hbox add failed: " + str(e))
        return None


def set_image_to_render_target(image_widget, rt_path):
    """Wire an Image widget to draw a render target asset."""
    if image_widget is None: return
    try:
        rt = EAS.load_asset(rt_path)
        if rt is None: return
        brush = image_widget.get_editor_property("brush")
        brush.set_editor_property("resource_object", rt)
        brush.set_editor_property("image_size", unreal.Vector2D(1024, 1024))
        image_widget.set_editor_property("brush", brush)
    except Exception as e:
        warn("RT brush wire failed: " + str(e))


def set_color(image_widget, color):
    try:
        brush = image_widget.get_editor_property("brush")
        brush.set_editor_property("tint_color", unreal.SlateColor(color))
        image_widget.set_editor_property("brush", brush)
    except Exception:
        pass


def compile_and_save(wbp):
    try:
        unreal.WidgetBlueprintLibrary.preview_widget(wbp) if hasattr(unreal, 'WidgetBlueprintLibrary') else None
    except Exception:
        pass
    try:
        EAS.save_asset(wbp.get_path_name())
    except Exception as e:
        warn("save failed: " + str(e))


# ---------------------------------------------------------------------------
# Concrete widget layouts
# ---------------------------------------------------------------------------

def build_HUD():
    wbp = EAS.load_asset("/Game/UI/WBP_HUD")
    tree = get_tree(wbp) if wbp else None
    if not tree: return
    root = construct(tree, unreal.CanvasPanel, "Root")
    if not root: return
    set_root(tree, root)

    # Top-left vertical box: stamina + ink bars
    vbox = construct(tree, unreal.VerticalBox, "TopLeftBox")
    add_to_canvas(root, vbox, anchors=(0,0,0,0), offsets=(20,20,260,80))

    stamina = construct(tree, unreal.ProgressBar, "Stamina_Bar")
    if stamina:
        try: stamina.set_editor_property("percent", 1.0)
        except Exception: pass
        vbox_add(vbox, stamina)

    ink = construct(tree, unreal.ProgressBar, "Ink_Bar")
    if ink:
        try: ink.set_editor_property("percent", 1.0)
        except Exception: pass
        vbox_add(vbox, ink)

    # Top-right time + region
    rvbox = construct(tree, unreal.VerticalBox, "TopRightBox")
    add_to_canvas(root, rvbox, anchors=(1,0,1,0), offsets=(-260,20,-20,80), alignment=(0,0))

    time_text   = construct(tree, unreal.TextBlock, "Time_Text")
    region_text = construct(tree, unreal.TextBlock, "Region_Text")
    if time_text:
        try: time_text.set_editor_property("text", "--:--")
        except Exception: pass
        vbox_add(rvbox, time_text)
    if region_text:
        try: region_text.set_editor_property("text", "")
        except Exception: pass
        vbox_add(rvbox, region_text)

    # Bottom-center prompt + tutorial hint
    bvbox = construct(tree, unreal.VerticalBox, "BottomCenterBox")
    add_to_canvas(root, bvbox, anchors=(0.5,1,0.5,1), offsets=(-300,-100,300,-20), alignment=(0.5,1))

    prompt = construct(tree, unreal.TextBlock, "Prompt_Text")
    hint   = construct(tree, unreal.TextBlock, "TutorialHint_Text")
    if prompt: vbox_add(bvbox, prompt)
    if hint:   vbox_add(bvbox, hint)

    compile_and_save(wbp)
    log("WBP_HUD laid out.")


def build_Compass():
    wbp = EAS.load_asset("/Game/UI/WBP_Compass")
    tree = get_tree(wbp) if wbp else None
    if not tree: return
    root = construct(tree, unreal.Overlay, "Root")
    if not root: return
    set_root(tree, root)

    arrow = construct(tree, unreal.Image, "Arrow_Image")
    if arrow:
        set_color(arrow, unreal.LinearColor(0.95, 0.92, 0.86, 1))
    try:
        slot = root.add_child_to_overlay(arrow)
        slot.set_editor_property("padding", unreal.Margin(0,0,0,0))
    except Exception: pass
    compile_and_save(wbp)
    log("WBP_Compass laid out.")


def build_DiscoveryToast():
    wbp = EAS.load_asset("/Game/UI/WBP_DiscoveryToast")
    tree = get_tree(wbp) if wbp else None
    if not tree: return
    root = construct(tree, unreal.CanvasPanel, "Root")
    if not root: return
    set_root(tree, root)

    box = construct(tree, unreal.VerticalBox, "ToastBox")
    add_to_canvas(root, box, anchors=(0.5,0,0.5,0), offsets=(-200,40,200,140), alignment=(0.5,0))

    title = construct(tree, unreal.TextBlock, "Title_Text")
    sub   = construct(tree, unreal.TextBlock, "Subtitle_Text")
    if title: vbox_add(box, title)
    if sub:   vbox_add(box, sub)

    compile_and_save(wbp)
    log("WBP_DiscoveryToast laid out.")


def build_FieldMap():
    wbp = EAS.load_asset("/Game/UI/WBP_FieldMap")
    tree = get_tree(wbp) if wbp else None
    if not tree: return
    root = construct(tree, unreal.CanvasPanel, "Root")
    if not root: return
    set_root(tree, root)

    parchment_bg = construct(tree, unreal.Image, "Parchment_Bg")
    if parchment_bg:
        set_color(parchment_bg, unreal.LinearColor(0.92, 0.86, 0.74, 1))
    add_to_canvas(root, parchment_bg, anchors=(0,0,1,1), offsets=(0,0,0,0))

    map_img = construct(tree, unreal.Image, "Img_Map")
    set_image_to_render_target(map_img, "/Game/RenderTargets/RT_FieldMap")
    add_to_canvas(root, map_img, anchors=(0,0,1,1), offsets=(60,60,-60,-60))

    title = construct(tree, unreal.TextBlock, "Title_Text")
    if title:
        try: title.set_editor_property("text", "Field Map")
        except Exception: pass
    add_to_canvas(root, title, anchors=(0.5,0,0.5,0), offsets=(-100,10,100,50), alignment=(0.5,0))

    compile_and_save(wbp)
    log("WBP_FieldMap laid out.")


def build_Inking():
    wbp = EAS.load_asset("/Game/UI/WBP_Inking")
    tree = get_tree(wbp) if wbp else None
    if not tree: return
    root = construct(tree, unreal.HorizontalBox, "Root")
    if not root: return
    set_root(tree, root)

    field = construct(tree, unreal.Image, "Field_Image")
    set_image_to_render_target(field, "/Game/RenderTargets/RT_FieldMap")
    if field:
        slot = hbox_add(root, field)
        try: slot.set_editor_property("size", make_fill_size())
        except Exception: pass

    master = construct(tree, unreal.Image, "Master_Image")
    set_image_to_render_target(master, "/Game/RenderTargets/RT_MasterMap")
    if master:
        slot = hbox_add(root, master)
        try: slot.set_editor_property("size", make_fill_size())
        except Exception: pass

    compile_and_save(wbp)
    log("WBP_Inking laid out.")


def build_Notebook():
    wbp = EAS.load_asset("/Game/UI/WBP_Notebook")
    tree = get_tree(wbp) if wbp else None
    if not tree: return
    root = construct(tree, unreal.CanvasPanel, "Root")
    if not root: return
    set_root(tree, root)

    bg = construct(tree, unreal.Image, "Notebook_Bg")
    if bg: set_color(bg, unreal.LinearColor(0.85, 0.79, 0.65, 1))
    add_to_canvas(root, bg, anchors=(0,0,1,1), offsets=(40,40,-40,-40))

    scroll = construct(tree, unreal.ScrollBox, "EntryScroll")
    add_to_canvas(root, scroll, anchors=(0,0,1,1), offsets=(80,100,-80,-80))

    title = construct(tree, unreal.TextBlock, "Title_Text")
    if title:
        try: title.set_editor_property("text", "Field Notebook")
        except Exception: pass
    add_to_canvas(root, title, anchors=(0.5,0,0.5,0), offsets=(-150,50,150,90), alignment=(0.5,0))

    compile_and_save(wbp)
    log("WBP_Notebook laid out.")


def build_Pause():
    wbp = EAS.load_asset("/Game/UI/WBP_Pause")
    tree = get_tree(wbp) if wbp else None
    if not tree: return
    root = construct(tree, unreal.CanvasPanel, "Root")
    if not root: return
    set_root(tree, root)

    bg = construct(tree, unreal.Image, "Bg")
    if bg: set_color(bg, unreal.LinearColor(0,0,0,0.5))
    add_to_canvas(root, bg, anchors=(0,0,1,1), offsets=(0,0,0,0))

    box = construct(tree, unreal.VerticalBox, "Buttons")
    add_to_canvas(root, box, anchors=(0.5,0.5,0.5,0.5), offsets=(-150,-150,150,150), alignment=(0.5,0.5))

    for label in ("Resume", "Save", "Load", "Settings", "Quit"):
        btn = construct(tree, unreal.Button, "Btn_" + label)
        txt = construct(tree, unreal.TextBlock, "Lbl_" + label)
        if txt:
            try: txt.set_editor_property("text", label)
            except Exception: pass
        if btn and txt:
            try: btn.add_child(txt)
            except Exception:
                try: btn.set_editor_property("content", txt)
                except Exception: pass
        if btn: vbox_add(box, btn, padding=(8,8,8,8))

    compile_and_save(wbp)
    log("WBP_Pause laid out.")


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def run():
    log("Building widget layouts")
    for fn in (build_HUD, build_Compass, build_DiscoveryToast, build_FieldMap,
               build_Inking, build_Notebook, build_Pause):
        try:
            fn()
        except Exception as e:
            warn("Layout step failed: " + str(e))


if __name__ == "__main__":
    run()
