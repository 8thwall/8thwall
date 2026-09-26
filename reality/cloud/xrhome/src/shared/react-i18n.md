# Typed React i18n usage

The React i18n types are declared in `react-i18n.d.ts`. Translation keys are
checked against the English JSON resources, and namespace selection is part of
the type check.

## Namespace selection

The first namespace passed to `useTranslation` is the implicit namespace. A
key may be used without an `ns` option only when it belongs to that namespace.

```tsx
const {t} = useTranslation(['cloud-editor-pages', 'common'])

t('editor_page.export_modal.build_type') // allowed: cloud-editor-pages
t('button.delete', {ns: 'common'})       // allowed: common is explicit
t('button.delete')                       // disallowed: common is not implicit
```

The same rule applies to a single namespace:

```tsx
const {t} = useTranslation(['cloud-studio-pages'])

t('mesh_configurator.title')             // allowed
t('button.delete', {ns: 'common'})       // disallowed: common was not requested
t('button.delete')                       // disallowed: wrong implicit namespace

const {t} = useTranslation(['cloud-studio-pages', 'common'])
t('button.delete', {ns: 'common'})       // allowed after requesting common
```

Using the string form is equivalent:

```tsx
const {t} = useTranslation('asset-lab')
t('asset_lab.title')                     // allowed
```

No namespace argument is allowed. Every hook must declare the namespaces it
uses:

```tsx
const {t} = useTranslation()              // disallowed
const {t} = useTranslation(['common'])    // allowed
t('button.delete')                       // allowed: common
```

`ns` must be one of the namespaces requested by the hook, and the key must
belong to that namespace:

```tsx
t('button.delete', {ns: 'common'})       // allowed
t('asset_lab.title', {ns: 'asset-lab'})  // allowed when asset-lab was requested
t('button.delete', {ns: 'asset-lab'})    // disallowed: common was not requested
t('button.delete', {ns: 'missing'})      // disallowed: unknown namespace
```

The supported namespaces are:

```text
account-pages
app-pages
asset-lab
caught-error-page
cloud-editor-pages
cloud-studio-pages
common
public-featured-pages
studio-desktop-pages
studio-tooltips
```

## Key syntax

Keys use the bare JSON key. Nested JSON objects are addressed with dot
separators:

```tsx
t('editor_page.export_modal.build_type')
t('editor_page.export_modal.build_type.apk_description')
```

Do not use the `namespace:key` syntax. Select the namespace with `useTranslation`
and/or the `ns` option instead:

```tsx
t('button.delete', {ns: 'common'})       // allowed
t('common:button.delete')                // disallowed
```

Plural resources support both the explicit suffixes and their base key. If a
resource contains `results_one` and `results_other`, all of these forms are
typed:

```tsx
t('tree_hierarchy_search_results.results_length_one')
t('tree_hierarchy_search_results.results_length_other')
t('tree_hierarchy_search_results.results_length')
```

The base form is only inferred for `_one` and `_other` pairs. It does not make
arbitrary missing keys valid.

## Invalid keys

Literal keys must exist in the selected namespace. Typos, keys from another
namespace, and keys removed from the JSON resources are errors:

```tsx
t('editor_page.export_modal.build_typ') // disallowed: typo
t('asset_lab.title')                    // disallowed in cloud-editor-pages
t('button.deleet', {ns: 'common'})      // disallowed: typo
```

## Dynamic keys

The type checker cannot prove an arbitrary `string` is a translation key. Avoid
constructing translation keys from unconstrained strings. For a finite dynamic
set, keep the mapping typed as a literal object:

```tsx
const keys = {
  apk: 'editor_page.export_modal.build_type.apk_description',
  aab: 'editor_page.export_modal.build_type.aab_description',
} as const

t(keys[buildType])
```

The mapping should cover every allowed value and should use the namespace that
contains the keys. Do not use a broad cast such as `as string` or `as any` to
bypass the translation-key check.
