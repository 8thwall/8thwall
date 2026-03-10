// @package(npm-ecs)
// @attr(externalize_npm = 1)
import {describe, it, assert, chai, chaiExclude} from '@repo/bzl/js/chai-js'

import {parseComponentAst} from './parse-component-ast'
import type {Schema} from './schema'
import type {StudioComponentData} from './studio-component'

chai.use(chaiExclude)

const createRegisterComponent = (
  name: string, schema: Schema, objectDec: boolean, varDec?: string
): [string, StudioComponentData] => {
  const varDeclaration = varDec ? `const ${varDec} = ` : ''
  const objectDeclaration = objectDec ? 'ecs.' : ''
  const modifiedSchema = Object.fromEntries(
    Object.entries(schema).map(([key, value]) => [key, `ecs.${value}`])
  )
  const callExpression = `registerComponent({
    name: '${name}',
    schema: ${JSON.stringify(modifiedSchema).replace(/"/g, '')},
  })\n`
  return [varDeclaration + objectDeclaration + callExpression, {
    name,
    schema,
    schemaDefaults: undefined,
    schemaPresentation: {fields: {}, groups: {}, sections: {}},
  } as const]
}

const IMPORT_STATEMENT = 'import ecs from \'@ecs/runtime\'\n'

const [VAR_DEC_1_DEC, VAR_DEC_1_DEC_DATA] = createRegisterComponent('spin', {
  x: 'f32', y: 'f32', z: 'f32', speed: 'f32',
}, true, 'Spin')

const [VAR_DEC_1_DEC_NO_OBJ, VAR_DEC_1_DEC_NO_OBJ_DATA] = createRegisterComponent('hop', {
  x: 'f32', y: 'f32', z: 'f32', speed: 'f32',
}, false, 'Hop')

const [EXP_STATE, EXP_DATA] = createRegisterComponent('dance', {
  x: 'f32', y: 'f32', z: 'f32', speed: 'f32',
}, false)

const VAR_DEC_2_DEC = `const Move = ecs.registerComponent({
  name: 'move',
  schema: {
    dx: ecs.f32,
    dy: ecs.f32,
    dz: ecs.f32,
    speed: ecs.f32,
  },
}),
Jump = ecs.registerComponent({
  name: 'jump',
  schema: {
    height: ecs.f32,
    speed: ecs.f32,
  },
})\n`

const VAR_DEC_ALL_PARAMS = `const Hide = registerComponent({
  name: 'hide',
  schema: {
    x: ecs.f32,
    y: ecs.f32,
    z: ecs.f32,
    a: ecs.f64,
    someBool: ecs.boolean,
    // @label Some String
    someString: ecs.string,
  },
  schemaDefaults: {
    x: 0,
    y: 1,
    z: -2,
    a: -0.5,
    someBool: true,
    someString: 'hello',
  },
  data: {
    dummy: 0,
  },
  add: (world, component) => {},
  tick: (world, component) => {},
  remove: (world, component) => {},
})`

const VAR_DEC_ALL_PARAMS_DATA: StudioComponentData[] = [
  {
    name: 'hide',
    schema: {x: 'f32', y: 'f32', z: 'f32', a: 'f64', someBool: 'boolean', someString: 'string'},
    schemaDefaults: {x: 0, y: 1, z: -2, a: -0.5, someBool: true, someString: 'hello'},
    schemaPresentation: {
      fields: {'someString': {'label': 'Some String'}}, groups: {}, sections: {},
    },
  },
]

const VAR_DEC_2_DEC_DATA: StudioComponentData[] = [
  {
    name: 'move',
    schema: {dx: 'f32', dy: 'f32', dz: 'f32', speed: 'f32'},
    schemaDefaults: undefined,
    schemaPresentation: {fields: {}, groups: {}, sections: {}},
  },
  {
    name: 'jump',
    schema: {height: 'f32', speed: 'f32'},
    schemaDefaults: undefined,
    schemaPresentation: {fields: {}, groups: {}, sections: {}},
  },
]

const RANDOM_VAR_DEC = 'const random = ecs.notAFunction({})\n'

describe('Cloud Studio - Parse Component Ast', () => {
  describe('When parsing a file with valid registerComponent code', () => {
    it('Should return the correct component data for a single variable declaration', () => {
      const content = IMPORT_STATEMENT + VAR_DEC_1_DEC
      assert.deepEqualExcluding(
        parseComponentAst(content).componentData, [VAR_DEC_1_DEC_DATA], 'location'
      )
    })
    it('Should return the correct component data for a single variable no object', () => {
      const content = IMPORT_STATEMENT + VAR_DEC_1_DEC_NO_OBJ
      assert.deepEqualExcluding(
        parseComponentAst(content).componentData, [VAR_DEC_1_DEC_NO_OBJ_DATA], 'location'
      )
    })
    it('Should return the correct component data for an expression statement', () => {
      const content = IMPORT_STATEMENT + EXP_STATE
      assert.deepEqualExcluding(
        parseComponentAst(content).componentData, [EXP_DATA], 'location'
      )
    })
    it('Should return the correct component data for multiple variable declarations', () => {
      const content = IMPORT_STATEMENT + VAR_DEC_2_DEC
      assert.deepEqualExcluding(
        parseComponentAst(content).componentData, VAR_DEC_2_DEC_DATA, 'location'
      )
    })
    it('Should return the correct component data with other functions inside', () => {
      const content = IMPORT_STATEMENT + RANDOM_VAR_DEC + VAR_DEC_1_DEC
      assert.deepEqualExcluding(
        parseComponentAst(content).componentData, [VAR_DEC_1_DEC_DATA], 'location'
      )
    })
    it('Should return the correct component data with other functions outside', () => {
      const content = RANDOM_VAR_DEC + IMPORT_STATEMENT + VAR_DEC_1_DEC
      assert.deepEqualExcluding(
        parseComponentAst(content).componentData, [VAR_DEC_1_DEC_DATA], 'location'
      )
    })
    it('Should return the correct component data with name and schema switched', () => {
      const content = `${IMPORT_STATEMENT}const a = ecs.registerComponent({
        schema: {x: ecs.f32, y: ecs.f32, z: ecs.f32, speed: ecs.f32},
        name: 'spin',
      })\n`
      assert.deepEqualExcluding(
        parseComponentAst(content).componentData, [VAR_DEC_1_DEC_DATA], 'location'
      )
    })
    it('Should return the correct components when there are multiple registerComponent calls',
      () => {
        const context = IMPORT_STATEMENT + VAR_DEC_1_DEC + VAR_DEC_1_DEC_NO_OBJ + EXP_STATE
        assert.deepEqualExcluding(
          parseComponentAst(context).componentData, [
            VAR_DEC_1_DEC_DATA, VAR_DEC_1_DEC_NO_OBJ_DATA, EXP_DATA,
          ], 'location'
        )
      })
    it('Should return the correct component data for all parameters', () => {
      const content = IMPORT_STATEMENT + VAR_DEC_ALL_PARAMS
      assert.deepEqualExcluding(
        parseComponentAst(content).componentData, VAR_DEC_ALL_PARAMS_DATA, 'location'
      )
    })
    it('Should return the correct component if schema property is an Identifier', () => {
      const content = `import {registerComponent, f64} from '@ecs/runtime'

      const DeleteAfter = registerComponent({
        name: 'delete-after',
        schema: {ms: f64},
        data: {start: f64},
      })

      export {
        DeleteAfter,
      }
      `
      assert.deepEqual(
        parseComponentAst(content).componentData,
        [{
          name: 'delete-after',
          schema: {ms: 'f64'},
          schemaDefaults: undefined,
          schemaPresentation: {fields: {}, groups: {}, sections: {}},
          location: {
            startLine: 3,
            startColumn: 26,
            endLine: 3,
            endColumn: 43,
          },
        }]
      )
    })
    it('Should return correct component if schema is not provided', () => {
      const content = `${IMPORT_STATEMENT}const a = ecs.registerComponent({
        name: 'spin',
      })\n`
      assert.deepEqual(
        parseComponentAst(content).componentData,
        [{
          name: 'spin',
          schema: {},
          schemaDefaults: undefined,
          location: {
            startLine: 2,
            startColumn: 14,
            endLine: 2,
            endColumn: 31,
          },
        }]
      )
    })
    it('Should return enum values', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @enum box, sphere, plane
          type: ecs.string,
        },
        schemaDefaults: {
          type: 'box',
        },
      })\n`
      assert.deepEqual(parseComponentAst(content).componentData,
        [{
          name: 'example',
          schema: {type: 'string'},
          schemaDefaults: {
            type: 'box',
          },
          schemaPresentation: {
            fields: {
              type: {
                mode: 'enum',
                enumValues: ['box', 'sphere', 'plane'],
              },
            },
            groups: {},
            sections: {},
          },
          location: {
            startLine: 1,
            startColumn: 4,
            endLine: 1,
            endColumn: 21,
          },
        }])
    })
    it('Should return enum values and labels', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @label Shape Type
          // @enum box, sphere, plane
          type: ecs.string,
        },
        schemaDefaults: {
          type: 'box',
        },
      })\n`
      assert.deepEqual(parseComponentAst(content).componentData,
        [{
          name: 'example',
          schema: {type: 'string'},
          schemaDefaults: {
            type: 'box',
          },
          schemaPresentation: {
            fields: {
              type: {
                mode: 'enum',
                label: 'Shape Type',
                enumValues: ['box', 'sphere', 'plane'],
              },
            },
            groups: {},
            sections: {},
          },
          location: {
            startLine: 1,
            startColumn: 4,
            endLine: 1,
            endColumn: 21,
          },
        }])
    })
    it('Should support @required for eid fields', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @required
          object: ecs.eid,
        },
      })\n`
      assert.deepEqual(parseComponentAst(content).componentData,
        [{
          name: 'example',
          schema: {object: 'eid'},
          schemaDefaults: undefined,
          schemaPresentation: {
            fields: {
              object: {
                required: true,
              },
            },
            groups: {},
            sections: {},
          },
          location: {
            startLine: 1,
            startColumn: 4,
            endLine: 1,
            endColumn: 21,
          },
        }])
    })
    it('Should reject the use of @required on non-eid fields', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @required
          object: ecs.string,
        },
      })\n`
      assert.isTrue(parseComponentAst(content).errors.length > 0)
    })
    it('Should support @color for string fields', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @color
          color: ecs.string,
        },
      })\n`
      assert.deepEqual(parseComponentAst(content).componentData,
        [{
          name: 'example',
          schema: {color: 'string'},
          schemaDefaults: undefined,
          schemaPresentation: {
            fields: {
              color: {
                mode: 'color',
              },
            },
            groups: {},
            sections: {},
          },
          location: {
            startLine: 1,
            startColumn: 4,
            endLine: 1,
            endColumn: 21,
          },
        }])
    })
    it('Should reject the use of @color on non-string fields', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @color
          color: ecs.f32,
        },
      })\n`
      assert.isTrue(parseComponentAst(content).errors.length > 0)
    })
    it('Should reject invalid @color suffixes', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @color hex
          color: ecs.string,
        },
      })\n`
      assert.isTrue(parseComponentAst(content).errors.length > 0)
    })
    it('Should ignore spacing around delimiter', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @enum 1,2, 3   , 4
          type: ecs.string,
        },
      })\n`
      assert.deepEqual(parseComponentAst(content).componentData,
        [{
          name: 'example',
          schema: {type: 'string'},
          schemaDefaults: undefined,
          schemaPresentation: {
            fields: {
              type: {
                mode: 'enum',
                enumValues: ['1', '2', '3', '4'],
              },
            },
            groups: {},
            sections: {},
          },
          location: {
            startLine: 1,
            startColumn: 4,
            endLine: 1,
            endColumn: 21,
          },
        }])
    })
    it('Should return an asset mode', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @asset
          src: ecs.string,
        },
      })\n`
      assert.deepEqual(parseComponentAst(content).componentData,
        [{
          name: 'example',
          schema: {src: 'string'},
          schemaDefaults: undefined,
          schemaPresentation: {
            fields: {
              src: {
                assetKind: undefined,
                mode: 'asset',
              },
            },
            groups: {},
            sections: {},
          },
          location: {
            startLine: 1,
            startColumn: 4,
            endLine: 1,
            endColumn: 21,
          },
        }])
    })
    it('Should return an asset of kind', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @asset model
          src: ecs.string,
        },
      })\n`
      assert.deepEqual(parseComponentAst(content).componentData,
        [{
          name: 'example',
          schema: {src: 'string'},
          schemaDefaults: undefined,
          schemaPresentation: {
            fields: {
              src: {
                assetKind: 'model',
                mode: 'asset',
              },
            },
            groups: {},
            sections: {},
          },
          location: {
            startLine: 1,
            startColumn: 4,
            endLine: 1,
            endColumn: 21,
          },
        }])
    })
    it('Should ignore unknown directives', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @something
          type: ecs.string,
        },
      })\n`
      assert.deepEqual(parseComponentAst(content).componentData,
        [{
          name: 'example',
          schema: {type: 'string'},
          schemaDefaults: undefined,
          schemaPresentation: {fields: {}, groups: {}, sections: {}},
          location: {
            startLine: 1,
            startColumn: 4,
            endLine: 1,
            endColumn: 21,
          },
        }])
    })
    it('Should return min and max values', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @min 1
          // @max 10
          src: ecs.i32,
        },
      })\n`
      assert.deepEqual(parseComponentAst(content).componentData,
        [{
          name: 'example',
          schema: {src: 'i32'},
          schemaDefaults: undefined,
          schemaPresentation: {
            fields: {
              src: {
                min: 1,
                max: 10,
              },
            },
            groups: {},
            sections: {},
          },
          location: {
            startLine: 1,
            startColumn: 4,
            endLine: 1,
            endColumn: 21,
          },
        }])
    })
    it('Should return min and max values for f32', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @min 1.3
          // @max 10.5
          src: ecs.f32,
        },
      })\n`
      assert.deepEqual(parseComponentAst(content).componentData,
        [{
          name: 'example',
          schema: {src: 'f32'},
          schemaDefaults: undefined,
          schemaPresentation: {
            fields: {
              src: {
                min: 1.3,
                max: 10.5,
              },
            },
            groups: {},
            sections: {},
          },
          location: {
            startLine: 1,
            startColumn: 4,
            endLine: 1,
            endColumn: 21,
          },
        }])
    })
    it('Should return min and max values for scientific notation', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @min 10e-3
          // @max 10e5
          src: ecs.f32,
        },
      })\n`
      assert.deepEqual(parseComponentAst(content).componentData,
        [{
          name: 'example',
          schema: {src: 'f32'},
          schemaDefaults: undefined,
          schemaPresentation: {
            fields: {
              src: {
                min: 10e-3,
                max: 10e5,
              },
            },
            groups: {},
            sections: {},
          },
          location: {
            startLine: 1,
            startColumn: 4,
            endLine: 1,
            endColumn: 21,
          },
        }])
    })
    it('Should return error if min is done incorrectly', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @min 0/@min 0/@min 0/@min 1
          src: ecs.i32,
        },
      })\n`
      assert.isTrue(parseComponentAst(content).errors.length > 0)
    })
    it('Should return error if max is done incorrectly', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @max 0/@min 0/@min 0/@min 1
          src: ecs.i32,
        },
      })\n`
      assert.isTrue(parseComponentAst(content).errors.length > 0)
    })
    it('Should return error if min is added twice', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @min 0
          // @min 1
          src: ecs.i32,
        },
      })\n`
      assert.isTrue(parseComponentAst(content).errors.length > 0)
    })
    it('Should return error if max is added twice', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @max 0
          // @max 1
          src: ecs.i32,
        },
      })\n`
      assert.isTrue(parseComponentAst(content).errors.length > 0)
    })
  })
  describe('When parsing a file with no registerComponent code', () => {
    it('Should return an empty array', () => {
      const content = `${IMPORT_STATEMENT}const a = 2\n`
      assert.deepEqual(parseComponentAst(content).componentData, [])
    })
    it('Should return an empty array for an empty file', () => {
      const content = ''
      assert.deepEqual(parseComponentAst(content).componentData, [])
    })
    it('Should return an empty array for a file with only comments', () => {
      const content = '// This is a comment\n/* This is another comment */'
      assert.deepEqual(parseComponentAst(content).componentData, [])
    })
    it('Should return an empty array for a file with code but no registerComponent', () => {
      const content = 'const a = 2;\nfunction test() { return a * 2; }\n'
      assert.deepEqual(parseComponentAst(content).componentData, [])
    })
  })
  describe('Throws parse errors when registerComponent has invalid parameters', () => {
    it('Should throw an error if name is not provided', () => {
      const content = `${IMPORT_STATEMENT}const a = ecs.registerComponent({
        schema: {x: ecs.f32, y: ecs.f32, z: ecs.f32, speed: ecs.f32},
      })\n`
      assert.isTrue(parseComponentAst(content).errors.length > 0)
    })
    it('Should throw an error if name is not a string', () => {
      const content = `${IMPORT_STATEMENT}const a = ecs.registerComponent({
        name: 5,
        schema: {x: ecs.f32, y: ecs.f32, z: ecs.f32, speed: ecs.f32},
      })\n`
      assert.isTrue(parseComponentAst(content).errors.length > 0)
    })
    it('Should throw an error if schema is not an object', () => {
      const content = `${IMPORT_STATEMENT}const a = ecs.registerComponent({
        name: 'spin',
        schema: 5,
      })\n`
      assert.isTrue(parseComponentAst(content).errors.length > 0)
    })
    it('Should throw an error if schema is a function declaration', () => {
      const content = `${IMPORT_STATEMENT}const a = ecs.registerComponent({
        name: 'spin',
        schema: function() { return 5; },
      })\n`
      assert.isTrue(parseComponentAst(content).errors.length > 0)
    })
    it('Should throw an error if there is more than 2 arguments', () => {
      const content = `${IMPORT_STATEMENT}const a = ecs.registerComponent({
        name: 'spin',
        schema: {x: ecs.f32, y: ecs.f32, z: ecs.f32, speed: ecs.f32},
      }, 5)\n`
      assert.isTrue(parseComponentAst(content).errors.length > 0)
    })
    it('Should throw an error if there are 2 name keys', () => {
      const content = `${IMPORT_STATEMENT}const a = ecs.registerComponent({
        name: 'spin',
        name: 'hop',
        schema: {x: ecs.f32, y: ecs.f32, z: ecs.f32, speed: ecs.f32},
      })\n`
      assert.isTrue(parseComponentAst(content).errors.length > 0)
    })
    it('Should throw an error if there is an invalid key', () => {
      const content = `${IMPORT_STATEMENT}const a = ecs.registerComponent({
        name: 'spin',
        schema: {x: ecs.f32, y: ecs.f32, z: ecs.f32, speed: ecs.f32},
        invalid: 5,
      })\n`
      assert.isTrue(parseComponentAst(content).errors.length > 0)
    })
    it('Should throw an error if there is an invalid enum value (tbd implementation)', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @enum "box", sphere, plane
          type: ecs.string,
        },
      })\n`
      assert.isTrue(parseComponentAst(content).errors.length > 0)
    })
    it('Should throw an error if there is a duplicate enum directive', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @enum box, sphere, plane
          // @enum box, sphere, plane
          type: ecs.string,
        },
      })\n`
      assert.isTrue(parseComponentAst(content).errors.length > 0)
    })
    it('Should throw an error if there is a -false default', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          type: ecs.boolean,
        },
        schema: {
          type: -false,
        },
      })\n`
      assert.isTrue(parseComponentAst(content).errors.length > 0)
    })
    it('Should throw an error if there is a duplicate field in the schema', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          type: ecs.string,
          type: ecs.string,
        },
      })\n`
      assert.deepEqualExcluding(parseComponentAst(content).errors[0],
        {
          message: 'Duplicate field \'type\'',
          severity: 'error',
        },
        'location')
    })
  })
  describe('Creates warnings when registerComponent has bad decorations', () => {
    it('Should create a warning if there are 2 @labels', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @label Shape Type
          // @label Shape Type
          type: ecs.string,
        },
      })\n`
      assert.deepEqualExcludingEvery(parseComponentAst(content).errors,
        [{
          message: 'expected only one @label for type',
          severity: 'warning',
        }], 'location')
    })
    it('Should create a warning if there are 2 @enums', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @enum box, sphere, plane
          // @enum box, sphere, plane
          type: ecs.string,
        },
      })\n`
      assert.deepEqualExcludingEvery(parseComponentAst(content).errors,
        [{
          message: 'expected only one @enum for type',
          severity: 'warning',
        }],
        'location')
    })
    it('Should crete a warning if there are 2 @conditions', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @condition box, sphere, plane
          // @condition box, sphere, plane
          type: ecs.string,
        },
      })\n`
      assert.deepEqualExcludingEvery(parseComponentAst(content).errors,
        [{
          message: 'expected only one @condition for type',
          severity: 'warning',
        }], 'location')
    })
    it('Should create 2 warnings if there are 2 @labels and 2 @enums', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @label Shape Type
          // @label Shape Type
          // @enum box, sphere, plane
          // @enum box, sphere, plane
          type: ecs.string,
        },
      })\n`
      assert.deepEqualExcludingEvery(parseComponentAst(content).errors,
        [
          {
            message: 'expected only one @label for type',
            severity: 'warning',
          },
          {
            message: 'expected only one @enum for type',
            severity: 'warning',
          },
        ], 'location')
    })
    it('Should create a warning if there is an @enum and an @attribute', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @enum box, sphere, plane
          // @attribute string
          type: ecs.string,
        },
      })\n`
      assert.deepEqualExcludingEvery(parseComponentAst(content).errors,
        [{
          message: 'cannot set type to @attribute (already set to enum)',
          severity: 'warning',
        }],
        'location')
    })
    it('Should create a warning if there is an @attribute and a property', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @attribute string
          // @property-of thing
          type: ecs.string,
        },
      })\n`
      assert.deepEqualExcludingEvery(parseComponentAst(content).errors,
        [{
          message: 'cannot set type to @property-of (already set to attribute)',
          severity: 'warning',
        }],
        'location')
    })
    it('Should create a warning if there is an @attribute and an @asset', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @enum 1,2
          // @asset
          type: ecs.string,
        },
      })\n`
      assert.deepEqualExcludingEvery(parseComponentAst(content).errors,
        [{
          message: 'cannot set type to @asset (already set to enum)',
          severity: 'warning',
        }],
        'location')
    })
    it('Should create a warning if there is an @attribute and an @asset', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @attribute string
          // @asset
          type: ecs.string,
        },
      })\n`
      assert.deepEqualExcludingEvery(parseComponentAst(content).errors,
        [{
          message: 'cannot set type to @asset (already set to attribute)',
          severity: 'warning',
        }],
        'location')
    })
    it('Should create a warning if there the @asset kind is invalid', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @asset invalid-kind
          someAsset: ecs.string,
        },
      })\n`
      assert.deepEqualExcludingEvery(parseComponentAst(content).errors,
        [{
          message: 'Unknown asset kind invalid-kind for field someAsset',
          severity: 'warning',
        }],
        'location')
    })
    it('Should create a warning if an @attribute type is invalid', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @attribute invalid-type
          type: ecs.string,
        },
      })\n`
      assert.deepEqualExcludingEvery(parseComponentAst(content).errors,
        [{
          message: 'expected one of number, string, boolean, vector3 for @attribute type',
          severity: 'warning',
        }],
        'location')
    })
    it('Should create a warning if a @property-of attribute does not exist', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @property-of undeclared-attribute
          type: ecs.string,
        },
      })\n`
      assert.deepEqualExcludingEvery(parseComponentAst(content).errors,
        [{
          message: '@property-of undeclared-attribute for type should be a valid field',
          severity: 'warning',
        }],
        'location')
    })
    it('Should create a warning if there is a duplicate @group', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @group start someGroup
          thing1: ecs.string,
          // @group end
          // @group start someGroup
        },
      })\n`
      assert.deepEqualExcludingEvery(parseComponentAst(content).errors,
        [{
          message: 'Duplicate @group of name someGroup',
          severity: 'warning',
        }],
        'location')
    })
    it('Should create a warning if there is a duplicate @section', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @section start someSection
          thing1: ecs.string,
          // @section end
          // @section start someSection
        },
      })\n`
      assert.deepEqualExcludingEvery(parseComponentAst(content).errors,
        [{
          message: 'Duplicate @section of name someSection',
          severity: 'warning',
        }],
        'location')
    })
    it('Should create a warning if there is a @group already in-progress', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @group start group1
          thing1: ecs.string,
          // @group start group2
          // @group end
        },
      })\n`
      assert.deepEqualExcludingEvery(parseComponentAst(content).errors,
        [{
          message: 'Cannot start @group group2. group group1 already in progress',
          severity: 'warning',
        }],
        'location')
    })
    it('Should create a warning if there is a @section already in-progress', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @section start section1
          thing1: ecs.string,
          // @section start section2
          // @section end
        },
      })\n`
      assert.deepEqualExcludingEvery(parseComponentAst(content).errors,
        [{
          message: 'Cannot start @section section2. section section1 already in progress',
          severity: 'warning',
        }],
        'location')
    })
    it('Should create a warning if there is no @group already in-progress', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @group start group1
          thing1: ecs.string,
          // @group end
          // @group end
        },
      })\n`
      assert.deepEqualExcludingEvery(parseComponentAst(content).errors,
        [{
          message: 'Cannot end @group. No @group in progress',
          severity: 'warning',
        }],
        'location')
    })
    it('Should create a warning if there is no @section already in-progress', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @section start section1
          thing1: ecs.string,
          // @section end
          // @section end
        },
      })\n`
      assert.deepEqualExcludingEvery(parseComponentAst(content).errors,
        [{
          message: 'Cannot end @section. No @section in progress',
          severity: 'warning',
        }],
        'location')
    })
    it('Should create a warning if too many fields in a vector3 group', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @group start someVectorGroup:vector3
          x: ecs.f32,
          y: ecs.f32,
          z: ecs.f32,
          zed: ecs.f32,
          // @group end
        },
      })\n`
      assert.deepEqualExcludingEvery(parseComponentAst(content).errors,
        [{
          message: 'expected 3 fields for group \'someVectorGroup\' of type vector3 (received 4)',
          severity: 'warning',
        }],
        'location')
    })
    it('Should create a warning if too few fields in a vector3 group', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @group start someVectorGroup:vector3
          x: ecs.f32,
          y: ecs.f32,
          // @group end
        },
      })\n`
      assert.deepEqualExcludingEvery(parseComponentAst(content).errors,
        [{
          message: 'expected 3 fields for group \'someVectorGroup\' of type vector3 (received 2)',
          severity: 'warning',
        }],
        'location')
    })
    it('Should create a warning if too many fields in a color group', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @group start someColorGroup:color
          r: ecs.f32,
          g: ecs.f32,
          b: ecs.f32,
          y: ecs.f32,
          // @group end
        },
      })\n`
      assert.deepEqualExcludingEvery(parseComponentAst(content).errors,
        [{
          message: 'expected 3 fields for group \'someColorGroup\' of type color (received 4)',
          severity: 'warning',
        }],
        'location')
    })
    it('Should create a warning if too few fields in a color group', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @group start someColorGroup:color
          r: ecs.f32,
          g: ecs.f32,
          // @group end
        },
      })\n`
      assert.deepEqualExcludingEvery(parseComponentAst(content).errors,
        [{
          message: 'expected 3 fields for group \'someColorGroup\' of type color (received 2)',
          severity: 'warning',
        }],
        'location')
    })
    it('Should create a warning if the wrong type of fields in a group', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @group start someVectorGroup:vector3
          x: ecs.f32,
          y: ecs.string,
          z: ecs.f32,
          // @group end
        },
      })\n`
      assert.deepEqualExcludingEvery(parseComponentAst(content).errors,
        [{
          // eslint-disable-next-line max-len
          message: 'expected a number type for property \'y\' in vector3 group \'someVectorGroup\' (received string)',
          severity: 'warning',
        }],
        'location')
    })
    it('Should create a warning if a group duplicates the name of a field', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          x: ecs.f32,
          // @group start x
        },
      })\n`
      assert.deepEqualExcludingEvery(parseComponentAst(content).errors,
        [{
          message: 'Cannot start @group \'x\'. Field of name \'x\' already exists',
          severity: 'warning',
        }],
        'location')
    })
    it('Should create a warning if a section duplicates the name of a field', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          x: ecs.f32,
          // @section start x
        },
      })\n`
      assert.deepEqualExcludingEvery(parseComponentAst(content).errors,
        [{
          message: 'Cannot start @section \'x\'. Field of name \'x\' already exists',
          severity: 'warning',
        }],
        'location')
    })
    it('Should create a warning if a field duplicates the name of a group', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @group start x
          xx: ecs.f32,
          // group end
          x: ecs.f32,
        },
      })\n`
      assert.deepEqualExcludingEvery(parseComponentAst(content).errors,
        [{
          message: 'Cannot start @group \'x\'. Field of name \'x\' already exists',
          severity: 'warning',
        }],
        'location')
    })
    it('Should create a warning if a field duplicates the name of a section', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          // @section start x
          xx: ecs.f32,
          // section end
          x: ecs.f32,
        },
      })\n`
      assert.deepEqualExcludingEvery(parseComponentAst(content).errors,
        [{
          message: 'Cannot start @section \'x\'. Field of name \'x\' already exists',
          severity: 'warning',
        }],
        'location')
    })
    it('Should create a warnings if there is a condition inside a group', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          x: ecs.boolean,
          // @group start someGroup
          // @condition x==true
          groupMember: ecs.string,
          // @group end
        },
      })\n`
      assert.deepEqualExcludingEvery(parseComponentAst(content).errors,
        [{
          message: 'Cannot set condition for field groupMember as it is part of group someGroup',
          severity: 'warning',
        }],
        'location')
    })
    it('Should create no warnings if there is a condition inside a section', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          x: ecs.boolean,
          // @section start someSection
          // @condition x==true
          groupMember: ecs.string,
          // @section end
        },
      })\n`
      assert.isTrue(parseComponentAst(content).errors.length === 0, 'location')
    })
    it('Should create no errors if there is a group inside a section', () => {
      const content = `ecs.registerComponent({
        name: 'example',
        schema: {
          x: ecs.boolean,
          // @section start someSection
          // @group start someGroup
          groupMember: ecs.string,
          // @group end
          // @section end
        },
      })\n`
      assert.isTrue(parseComponentAst(content).errors.length === 0, 'location')
    })
  })
  it('Should create a warning if there is a section started inside a group', () => {
    const content = `ecs.registerComponent({
      name: 'example',
      schema: {
        // @group start someGroup
        groupMember: ecs.string,
        // @section start someSection
        // @group end
      },
    })\n`
    assert.deepEqualExcludingEvery(parseComponentAst(content).errors,
      [{
        message: 'Cannot create section someSection inside group someGroup',
        severity: 'warning',
      }],
      'location')
  })
  it('Should create a warning if there is a section ended inside a group', () => {
    const content = `ecs.registerComponent({
      name: 'example',
      schema: {
        // @section start someSection
        sectionMember: ecs.string,
        // @group start someGroup
        groupMember1: ecs.string,
        // @section end
        groupMember2: ecs.string,
        // @group end
      },
    })\n`
    assert.deepEqualExcluding(parseComponentAst(content).errors[0],
      {
        message: 'Cannot end @section someSection inside group someGroup',
        severity: 'warning',
      },
      'location')
  })
  it('Should create a warning if a group has no end', () => {
    const content = `ecs.registerComponent({
      name: 'example',
      schema: {
        // @group start someGroup
        groupMember: ecs.string,
      },
    })\n`
    assert.deepEqualExcludingEvery(parseComponentAst(content).errors,
      [{
        message: 'Expected \'@group end\' for group someGroup',
        severity: 'warning',
      }],
      'location')
  })
  it('Should create a warning if a section has no end', () => {
    const content = `ecs.registerComponent({
      name: 'example',
      schema: {
        // @section start someSection
        sectionMember: ecs.string,
      },
    })\n`
    assert.deepEqualExcludingEvery(parseComponentAst(content).errors,
      [{
        message: 'Expected \'@section end\' for section someSection',
        severity: 'warning',
      }],
      'location')
  })
  it('Should create a warning if a @group type is invalid', () => {
    const content = `ecs.registerComponent({
      name: 'example',
      schema: {
        // @group start someGroup:invalidGroupType
        groupMember: ecs.string,
        // @group end
      },
    })\n`
    assert.deepEqualExcluding(parseComponentAst(content).errors[0],
      {
        message: 'expected one of vector3, color for @group type',
        severity: 'warning',
      },
      'location')
  })
  it('Should create a warning if a @section property is invalid', () => {
    const content = `ecs.registerComponent({
      name: 'example',
      schema: {
        // @section start someSection:invalidSectionProperty
        sectionMember: ecs.string,
        // @section end
      },
    })\n`
    assert.deepEqualExcluding(parseComponentAst(content).errors[0],
      {
        message: 'Unknown section property invalidSectionProperty',
        severity: 'warning',
      },
      'location')
  })
  it('Should create a warning if a @group label is outside of a group', () => {
    const content = `ecs.registerComponent({
      name: 'example',
      schema: {
        // @group label someLabel
        notAGroupMember: ecs.string,
      },
    })\n`
    assert.deepEqualExcluding(parseComponentAst(content).errors[0],
      {
        message: 'Cannot set label for group as no group in progress',
        severity: 'warning',
      },
      'location')
  })
  it('Should create a warning if a @section label is outside of a section', () => {
    const content = `ecs.registerComponent({
      name: 'example',
      schema: {
        // @section label someLabel
        notASectionMember: ecs.string,
      },
    })\n`
    assert.deepEqualExcluding(parseComponentAst(content).errors[0],
      {
        message: 'Cannot set label for section as no section in progress',
        severity: 'warning',
      },
      'location')
  })
  it('Should create a warning if a defaultClosed is outside of a section', () => {
    const content = `ecs.registerComponent({
      name: 'example',
      schema: {
        // @section defaultClosed
        notASectionMember: ecs.string,
      },
    })\n`
    assert.deepEqualExcluding(parseComponentAst(content).errors[0],
      {
        message: 'Cannot set defaultClosed for section as no section in progress',
        severity: 'warning',
      },
      'location')
  })
  it('Should create a warning if a @group condition is outside of a group', () => {
    const content = `ecs.registerComponent({
      name: 'example',
      schema: {
        someField: ecs.boolean,
        // @group condition someField=true
        notAGroupMember: ecs.string,
      },
    })\n`
    assert.deepEqualExcluding(parseComponentAst(content).errors[0],
      {
        message: 'Cannot set condition for group as no group in progress',
        severity: 'warning',
      },
      'location')
  })
  it('Should create a warning if a @section condition is outside of a section', () => {
    const content = `ecs.registerComponent({
      name: 'example',
      schema: {
        someField: ecs.boolean,
        // @section condition someField=true
        notASectionMember: ecs.string,
      },
    })\n`
    assert.deepEqualExcluding(parseComponentAst(content).errors[0],
      {
        message: 'Cannot set condition for section as no section in progress',
        severity: 'warning',
      },
      'location')
  })
  it('Should create a warning if a @group presentation element is unknown', () => {
    const content = `ecs.registerComponent({
      name: 'example',
      schema: {
        // @group start someGroup
        // @group unknownElement
        groupMember: ecs.string,
        // @group end
      },
    })\n`
    assert.deepEqualExcluding(parseComponentAst(content).errors[0],
      {
        message: 'Unknown group presentation element \'unknownElement\'',
        severity: 'warning',
      },
      'location')
  })
  it('Should create a warning if a @section presentation element is unknown', () => {
    const content = `ecs.registerComponent({
      name: 'example',
      schema: {
        // @section start someSection
        // @section unknownElement
        sectionMember: ecs.string,
        // @section end
      },
    })\n`
    assert.deepEqualExcluding(parseComponentAst(content).errors[0],
      {
        message: 'Unknown section presentation element \'unknownElement\'',
        severity: 'warning',
      },
      'location')
  })
  it('Should parse section labels', () => {
    const content = `ecs.registerComponent({
      name: 'example',
      schema: {
        // @section start someSection
        // @section label A Labelled Section
        sectionMember: ecs.string,
        // @section end
      },
    })\n`
    assert.deepEqual(parseComponentAst(content).componentData[0].schemaPresentation?.sections,
      {someSection: {label: 'A Labelled Section'}})
  })
  it('Should parse group labels', () => {
    const content = `ecs.registerComponent({
      name: 'example',
      schema: {
        // @group start someGroup
        // @group label A Labelled Group
        groupMember: ecs.string,
        // @group end
      },
    })\n`
    assert.deepEqual(parseComponentAst(content).componentData[0].schemaPresentation?.groups,
      {someGroup: {label: 'A Labelled Group'}})
  })
  it('Should parse section conditions', () => {
    const content = `ecs.registerComponent({
      name: 'example',
      schema: {
        someField: ecs.boolean,
        // @section start someSection
        // @section condition someField==true
        sectionMember: ecs.string,
        // @section end
      },
    })\n`
    assert.deepEqual(parseComponentAst(content).componentData[0].schemaPresentation?.sections,
      {someSection: {condition: 'someField==true'}})
  })
  it('Should parse group conditions', () => {
    const content = `ecs.registerComponent({
      name: 'example',
      schema: {
        someField: ecs.boolean,
        // @group start someGroup
        // @group condition someField==true
        groupMember: ecs.string,
        // @group end
      },
    })\n`
    assert.deepEqual(parseComponentAst(content).componentData[0].schemaPresentation?.groups,
      {someGroup: {condition: 'someField==true'}})
  })

  it('Should parse a component alongside a class', () => {
    const content = `
      import * as ecs from '@8thwall/ecs'

  const testComponent = ecs.registerComponent({name: 'test'})

  class UnusedClassFoo {
    Foo: typeof testComponent[] = []
  }
      `

    assert.isNotEmpty(parseComponentAst(content).componentData)
  })

  it('Should support extended schema to allow for inline defaults', () => {
    const content = `ecs.registerComponent({
      name: 'example',
      schema: {
        v1: [ecs.f32, 1.5],
        v2: [ecs.f32],
        v3: ecs.f32,
        v4: [ecs.string, 'test'],
      },
    })\n`
    assert.deepEqual(parseComponentAst(content),
      {
        componentData: [{
          name: 'example',
          schema: {v1: 'f32', v2: 'f32', v3: 'f32', v4: 'string'},
          schemaDefaults: {v1: 1.5, v4: 'test'},
          schemaPresentation: {
            fields: {},
            groups: {},
            sections: {},
          },
          location: {
            startLine: 1,
            startColumn: 4,
            endLine: 1,
            endColumn: 21,
          },
        }],
        errors: [],
      })
  })

  it('Should not allow schemaDefaults and inline defaults at the same time', () => {
    const content = `ecs.registerComponent({
      name: 'example',
      schema: {
        v1: [ecs.f32, 1.5],
        v2: [ecs.f32],
        v3: ecs.f32,
        v4: [ecs.f32],
      },
      schemaDefaults: {
        v2: 2.5,
      },
    })\n`
    assert.deepEqualExcluding(parseComponentAst(content).errors[0],
      {
        message: 'Cannot use schemaDefaults and defaults within schema at the same time',
        severity: 'error',
      },
      'location')
  })

  it('Should allow export const ecs.registerComponent', () => {
    const content = `export const testComponent = ecs.registerComponent({
      name: 'example',
      schema: {
        v1: ecs.f32,
      },
    })\n`
    assert.deepEqualExcluding(parseComponentAst(content).componentData,
      [{
        name: 'example',
        schema: {
          v1: 'f32',
        },
        schemaDefaults: undefined,
        schemaPresentation: {
          fields: {},
          groups: {},
          sections: {},
        },
      }],
      'location')
  })

  it('Should allow export const with multiple variable declarations', () => {
    const content = `export const
      Component1 = ecs.registerComponent({
        name: 'comp1',
        schema: { value: ecs.i32 },
      }),
      Component2 = ecs.registerComponent({
        name: 'comp2',
        schema: { flag: ecs.boolean },
      })\n`
    assert.deepEqualExcluding(parseComponentAst(content).componentData,
      [
        {
          name: 'comp1',
          schema: {value: 'i32'},
          schemaDefaults: undefined,
          schemaPresentation: {
            fields: {},
            groups: {},
            sections: {},
          },
        },
        {
          name: 'comp2',
          schema: {flag: 'boolean'},
          schemaDefaults: undefined,
          schemaPresentation: {
            fields: {},
            groups: {},
            sections: {},
          },
        },
      ],
      'location')
  })

  it('Should handle mixed export and regular declarations', () => {
    const content = `
    const regularComponent = ecs.registerComponent({
      name: 'regular',
      schema: { value: ecs.i32 },
    })

    export const exportedComponent = ecs.registerComponent({
      name: 'exported',
      schema: { flag: ecs.boolean },
    })

    ecs.registerComponent({
      name: 'expression',
      schema: { data: ecs.string },
    })\n`
    assert.deepEqualExcluding(parseComponentAst(content).componentData,
      [
        {
          name: 'regular',
          schema: {value: 'i32'},
          schemaDefaults: undefined,
          schemaPresentation: {
            fields: {},
            groups: {},
            sections: {},
          },
        },
        {
          name: 'exported',
          schema: {flag: 'boolean'},
          schemaDefaults: undefined,
          schemaPresentation: {
            fields: {},
            groups: {},
            sections: {},
          },
        },
        {
          name: 'expression',
          schema: {data: 'string'},
          schemaDefaults: undefined,
          schemaPresentation: {
            fields: {},
            groups: {},
            sections: {},
          },
        },
      ],
      'location')
  })
})
