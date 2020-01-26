## Scene file format

The level files are in JSON format, and allow for a basic description of a scene, 
with some parenting and data inheritance support. 

### Scene

TODO: "parent_id" is unsupported, need to implement it in `scene.hpp`.

A scene is a collection of entities. Child relationships can be represented
as an an entity id:

```json
{
  "entities": [
    {
      "entity_id": 1,
      "components": []
    },
    {
      "entity_id": 2,
      "parent_id": 1,
      "components": []
    }]
}
```

All the ids in these files should be set, since they ought to be generated 
by an editor eventually. But I guess there's no real reason not to auto-generate
and id if one is omitted.


### Components

Components are identified by the string passed to register_component();

The component_loader should be implemented by various systems that load and initialize entity components in various ways.
These loaders will have the json data from the scene file available to them, along with the ecs entity reference. 


Example:
```
 "components": [
        {
          "type": "transform",
          "position": [ 1, 2, 3 ],
          "rotation": [ 3.14, 3.14, 3.14 ],
          "scale": [ 1, 2, 3 ],
          "local_to_world": [ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 ],
          "is_matrix_dirty": false,
          "parent": 0,
          "has_parent": false
        },
        {
          "type": "renderable_mesh_static",
          "mesh": "this_isnt_a_file.fbx",
          "material": "this_isnt_a_file.mat"
        }
      ]
    }
```

### Prototypes

A prototype is like a Unity prefab. It is a template for some entity, which can have
children entities. Its format is:
```
{
    "root": {},
    "children": []
}
```
Where the root object and each `children` element are entities. These do not have ids.
The elements of "children" have the `root` entity as their implied parent. 
They can, alternatively, define parents that are within the `children` array
by index. So the property is called `parent_index` 