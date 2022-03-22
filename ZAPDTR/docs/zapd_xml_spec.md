# ZAPD XML specification

ZAPD XMLs use a restrictive subset of the XML standard: any ZAPD XML must be a valid XML (All elements starting with `<tag>` ending appropriately with `</tag>`, single "empty-element" tags with `/` at the end, etc.).

Reminder that in

```xml
<a>
    <b1>
        <c/>
    </b1>
    <b2>
        <d/>
    </b2>
    <e/>
</a>
```

`<b1>`, `<b2>`, `<e/>` are *children* of `<a>`, but `<c/>` is not. `<c/>` is a *descendent* of `<a>` and a child of `<b>`.

- Every XML's outermost element start/end tag is a single `<Root>`.
- The children of a `<Root>` must be `<File>`s.
- A `<File>` has *resources* as children. A resource is almost always single empty-element tag, and has one of the types
  - `<Texture>`
  - `<Background>`
  - `<Blob>`
  - `<DList>`
  - `<TextureAnimation>`
  - `<Scene>`
  - `<Room>`
  - `<AltHeader>`
  - `<Animation>`
  - `<PlayerAnimation>`
  - `<CurveAnimation>`
  - `<LegacyAnimation>`
  - `<Skeleton>`
  - `<LimbTable>`
  - `<Limb>`
  - `<Symbol>`
  - `<Collision>`
  - `<Scalar>`
  - `<Vector>`
  - `<Vtx>`
  - `<Mtx>`
  - `<Cutscene>`
  - `<Array>`
  - `<Path>`
  - `<PlayerAnimationData>`

- A `<File>` cannot descend from a `<File>`.
- All resources must be children of a `<File>`.
- `<Array>` is the only paired resource tag enclosing an element; the element must be a single resource tag, one of
  - `<Scalar>`
  - `<Vector>`
  - `<Vtx>`
