import { AmbientLight, AnimationClip, BufferGeometry, Camera as Camera$1, Color as Color$1, ColorSpace, DirectionalLight, Euler, Event as Event$1, Group, Intersection, MagnificationTextureFilter, Material as Material$1, Matrix4, Mesh, MeshStandardMaterial, MinificationTextureFilter, Object3D as OriginalObject3D, Object3DEventMap, OrthographicCamera, PerspectiveCamera, PointLight, Quaternion, Raycaster, RectAreaLight, Scene, SpotLight, Texture, TextureLoader, Vector3, Vector4, WebGLRenderList, WebGLRenderTarget, WebGLRenderer } from 'three';
import { GLTF } from 'three/examples/jsm/loaders/GLTFLoader.js';
import { DeepReadonly } from 'ts-essentials';

type AlignContent = "flex-start" | "flex-end" | "center" | "stretch" | "space-between" | "space-around" | "space-evenly";
type AlignItems = "flex-start" | "flex-end" | "center" | "stretch" | "baseline";
type JustifyContent = "flex-start" | "flex-end" | "center" | "space-between" | "space-around" | "space-evenly";
type TextAlignContent = "left" | "center" | "right" | "justify";
type VerticalTextAlignContent = "start" | "center" | "end";
type FlexWrap = "nowrap" | "wrap" | "wrap-reverse";
type FlexDirection = "row" | "column" | "row-reverse" | "column-reverse";
type Direction = "ltr" | "rtl";
type Display = "none" | "flex";
type Overflow = "visible" | "hidden" | "scroll";
type PositionMode = "absolute" | "relative" | "static";
type Major = {
	major: number;
};
type Minor = {
	minor: number;
};
type Patch = {
	patch: number;
};
type P<T> = Partial<T>;
type RuntimeVersionTarget = {
	type: "version";
	level: "major";
} & Major & P<Minor> & P<Patch> | {
	type: "version";
	level: "minor";
} & Major & Minor & P<Patch> | {
	type: "version";
	level: "patch";
} & Major & Minor & Patch;
type Vec2Tuple = [
	number,
	number
];
type Vec3Tuple = [
	number,
	number,
	number
];
type Vec4Tuple = [
	number,
	number,
	number,
	number
];
type Vec6Tuple = [
	number,
	number,
	number,
	number,
	number,
	number
];
type ObjectId = string;
type GraphComponent = {
	id: ObjectId;
	name: string;
	parameters: Record<string, string | number | boolean | EntityReference>;
};
type BoxGeometry = {
	type: "box";
	width: number;
	height: number;
	depth: number;
};
type SphereGeometry = {
	type: "sphere";
	radius: number;
};
type PlaneGeometry = {
	type: "plane";
	width: number;
	height: number;
};
type CapsuleGeometry = {
	type: "capsule";
	radius: number;
	height: number;
};
type ConeGeometry = {
	type: "cone";
	radius: number;
	height: number;
};
type CylinderGeometry = {
	type: "cylinder";
	radius: number;
	height: number;
};
type TetrahedronGeometry = {
	type: "tetrahedron";
	radius: number;
};
type Faces = 4 | 8 | 12 | 20;
type PolyhedronGeometry = {
	type: "polyhedron";
	radius: number;
	faces: Faces;
};
type CircleGeometry = {
	type: "circle";
	radius: number;
};
type RingGeometry = {
	type: "ring";
	innerRadius: number;
	outerRadius: number;
};
type TorusGeometry = {
	type: "torus";
	radius: number;
	tubeRadius: number;
};
type FaceGeometry = {
	type: "face";
	id: number;
};
type Side = "front" | "back" | "double";
type MaterialBlending = "no" | "normal" | "additive" | "subtractive" | "multiply";
type TextureWrap = "clamp" | "repeat" | "mirroredRepeat";
type TextureFiltering = "smooth" | "sharp";
type HiderMaterial = {
	type: "hider";
};
type ShadowMaterial = {
	type: "shadow";
	color: string;
	opacity?: number;
	side?: Side;
	depthTest?: boolean;
	depthWrite?: boolean;
};
type UnlitMaterial = {
	type: "unlit";
	color: string;
	textureSrc?: string | Resource;
	opacity?: number;
	opacityMap?: string | Resource;
	side?: Side;
	blending?: MaterialBlending;
	repeatX?: number;
	repeatY?: number;
	offsetX?: number;
	offsetY?: number;
	wrap?: TextureWrap;
	depthTest?: boolean;
	depthWrite?: boolean;
	wireframe?: boolean;
	forceTransparent?: boolean;
	textureFiltering?: TextureFiltering;
	mipmaps?: boolean;
};
type BasicMaterial = {
	type: "basic";
	color: string;
	textureSrc?: string | Resource;
	roughness?: number;
	metalness?: number;
	opacity?: number;
	normalScale?: number;
	emissiveIntensity?: number;
	roughnessMap?: string | Resource;
	metalnessMap?: string | Resource;
	opacityMap?: string | Resource;
	normalMap?: string | Resource;
	emissiveMap?: string | Resource;
	emissiveColor?: string;
	side?: Side;
	blending?: MaterialBlending;
	repeatX?: number;
	repeatY?: number;
	offsetX?: number;
	offsetY?: number;
	wrap?: TextureWrap;
	depthTest?: boolean;
	depthWrite?: boolean;
	wireframe?: boolean;
	forceTransparent?: boolean;
	textureFiltering?: TextureFiltering;
	mipmaps?: boolean;
};
type VideoMaterial = {
	type: "video";
	color: string;
	textureSrc?: string | Resource;
	opacity?: number;
};
type EntityReference = {
	type: "entity";
	id: ObjectId;
};
type Geometry = BoxGeometry | SphereGeometry | PlaneGeometry | CapsuleGeometry | ConeGeometry | CylinderGeometry | TetrahedronGeometry | PolyhedronGeometry | CircleGeometry | RingGeometry | TorusGeometry | FaceGeometry | null;
type Material = BasicMaterial | UnlitMaterial | ShadowMaterial | HiderMaterial | VideoMaterial | null;
type Url = {
	type: "url";
	url: string;
};
type Asset = {
	type: "asset";
	asset: string;
};
type Resource = Url | Asset;
type SimplificationMode = "convex" | "concave";
type GltfModel = {
	src: Resource;
	animationClip?: string;
	loop?: boolean;
	paused?: boolean;
	timeScale?: number;
	reverse?: boolean;
	repetitions?: number;
	crossFadeDuration?: number;
};
type Splat = {
	src: Resource;
	skybox?: boolean;
};
type ColliderType = "static" | "dynamic" | "kinematic";
type Collider = {
	type?: ColliderType;
	geometry?: BoxGeometry | SphereGeometry | PlaneGeometry | CapsuleGeometry | ConeGeometry | CylinderGeometry | {
		type: "auto";
	};
	mass?: number;
	linearDamping?: number;
	angularDamping?: number;
	friction?: number;
	rollingFriction?: number;
	spinningFriction?: number;
	restitution?: number;
	eventOnly?: boolean;
	lockXPosition?: boolean;
	lockYPosition?: boolean;
	lockZPosition?: boolean;
	lockXAxis?: boolean;
	lockYAxis?: boolean;
	lockZAxis?: boolean;
	gravityFactor?: number;
	highPrecision?: boolean;
	offsetX?: number;
	offsetY?: number;
	offsetZ?: number;
	offsetQuaternionX?: number;
	offsetQuaternionY?: number;
	offsetQuaternionZ?: number;
	offsetQuaternionW?: number;
	simplificationMode?: SimplificationMode;
};
type FontResource = {
	type: "font";
	font: string;
} | Resource;
type UiRootType = "overlay" | "3d";
type BackgroundSize = "contain" | "cover" | "stretch" | "nineslice";
type UiGraphSettings = Partial<{
	top: string;
	left: string;
	right: string;
	bottom: string;
	width: number | string;
	height: number | string;
	type: UiRootType;
	ignoreRaycast: boolean;
	font: FontResource;
	background: string;
	backgroundOpacity: number;
	backgroundSize: BackgroundSize;
	nineSliceBorderTop: string;
	nineSliceBorderBottom: string;
	nineSliceBorderLeft: string;
	nineSliceBorderRight: string;
	nineSliceScaleFactor: number;
	opacity: number;
	color: string;
	text: string;
	textAlign: TextAlignContent;
	verticalTextAlign: VerticalTextAlignContent;
	image: Resource;
	fixedSize: boolean;
	borderColor: string;
	borderRadius: number;
	borderRadiusTopLeft: string;
	borderRadiusTopRight: string;
	borderRadiusBottomRight: string;
	borderRadiusBottomLeft: string;
	fontSize: number;
	alignContent: AlignContent;
	alignItems: AlignItems;
	alignSelf: AlignItems;
	borderWidth: number;
	direction: Direction;
	display: Display;
	flex: number;
	flexBasis: string;
	flexDirection: FlexDirection;
	rowGap: string;
	gap: string;
	columnGap: string;
	flexGrow: number;
	flexShrink: number;
	flexWrap: FlexWrap;
	justifyContent: JustifyContent;
	margin: string;
	marginBottom: string;
	marginLeft: string;
	marginRight: string;
	marginTop: string;
	maxHeight: string;
	maxWidth: string;
	minHeight: string;
	minWidth: string;
	overflow: Overflow;
	padding: string;
	paddingBottom: string;
	paddingLeft: string;
	paddingRight: string;
	paddingTop: string;
	position: PositionMode;
	stackingOrder: number;
}>;
type Shadow = {
	castShadow?: boolean;
	receiveShadow?: boolean;
};
type DistanceModel = "exponential" | "inverse" | "linear";
type AudioSettings = {
	src?: Resource;
	volume?: number;
	loop?: boolean;
	paused?: boolean;
	pitch?: number;
	positional?: boolean;
	refDistance?: number;
	rolloffFactor?: number;
	distanceModel?: DistanceModel;
	maxDistance?: number;
};
type VideoControlsGraphSettings = {
	volume?: number;
	loop?: boolean;
	paused?: boolean;
	speed?: number;
	positional?: boolean;
	refDistance?: number;
	rolloffFactor?: number;
	distanceModel?: DistanceModel;
	maxDistance?: number;
};
type LightType = "directional" | "ambient" | "point" | "spot" | "area";
type Light = {
	type: LightType;
	color?: string;
	intensity?: number;
	castShadow?: boolean;
	target?: Vec3Tuple;
	shadowNormalBias?: number;
	shadowBias?: number;
	shadowAutoUpdate?: boolean;
	shadowBlurSamples?: number;
	shadowRadius?: number;
	shadowMapSize?: Vec2Tuple;
	shadowCamera?: Vec6Tuple;
	distance?: number;
	decay?: number;
	followCamera?: boolean;
	angle?: number;
	penumbra?: number;
	colorMap?: Resource;
	width?: number;
	height?: number;
};
type CameraType = "perspective" | "orthographic";
type Camera = {
	type?: CameraType;
	left?: number;
	right?: number;
	top?: number;
	bottom?: number;
	fov?: number;
	zoom?: number;
	nearClip?: number;
	farClip?: number;
	xr?: XrConfig;
};
type XrCameraType = "world" | "face" | "hand" | "layers" | "worldLayers" | "3dOnly";
type DeviceSupportType = "AR" | "VR" | "3D" | "disabled";
type CameraDirectionType = "front" | "back";
type XrConfig = {
	xrCameraType?: XrCameraType;
	phone?: DeviceSupportType;
	desktop?: DeviceSupportType;
	headset?: DeviceSupportType;
	leftHandedAxes?: boolean;
	uvType?: "standard" | "projected";
	direction?: CameraDirectionType;
	world?: XrWorldConfig;
	face?: XrFaceConfig;
};
type XrWorldConfig = {
	scale?: "absolute" | "responsive";
	disableWorldTracking?: boolean;
	enableLighting?: boolean;
	enableWorldPoints?: boolean;
	enableVps?: boolean;
};
type XrFaceConfig = {
	mirroredDisplay?: boolean;
	meshGeometryFace?: boolean;
	meshGeometryEyes?: boolean;
	meshGeometryIris?: boolean;
	meshGeometryMouth?: boolean;
	enableEars?: boolean;
	maxDetections?: number;
};
type Face = {
	id: number;
	addAttachmentState: boolean;
};
type StaticImageTargetOrientation = {
	rollAngle: number;
	pitchAngle: number;
};
type ImageTarget = {
	name: string;
	staticOrientation?: StaticImageTargetOrientation;
};
type LocationVisualization = "mesh" | "splat" | "none";

type Map$1 = {
	latitude: number;
	longitude: number;
	targetEntity?: EntityReference;
	radius: number;
	spawnLocations: boolean;
	useGps: boolean;
};
type MapTheme = {
	landColor?: string;
	buildingColor?: string;
	parkColor?: string;
	parkingColor?: string;
	roadColor?: string;
	sandColor?: string;
	transitColor?: string;
	waterColor?: string;
	landOpacity?: number;
	buildingOpacity?: number;
	parkOpacity?: number;
	parkingOpacity?: number;
	roadOpacity?: number;
	sandOpacity?: number;
	transitOpacity?: number;
	waterOpacity?: number;
	lod?: number;
	buildingBase?: number;
	parkBase?: number;
	parkingBase?: number;
	roadBase?: number;
	sandBase?: number;
	transitBase?: number;
	waterBase?: number;
	buildingMinMeters?: number;
	buildingMaxMeters?: number;
	roadLMeters?: number;
	roadMMeters?: number;
	roadSMeters?: number;
	roadXLMeters?: number;
	transitMeters?: number;
	waterMeters?: number;
	roadLMin?: number;
	roadMMin?: number;
	roadSMin?: number;
	roadXLMin?: number;
	transitMin?: number;
	waterMin?: number;
	landVisibility?: boolean;
	buildingVisibility?: boolean;
	parkVisibility?: boolean;
	parkingVisibility?: boolean;
	roadVisibility?: boolean;
	sandVisibility?: boolean;
	transitVisibility?: boolean;
	waterVisibility?: boolean;
};
type MapPoint = {
	latitude: number;
	longitude: number;
	targetEntity?: EntityReference;
	meters: number;
	minScale: number;
};
type BaseGraphObject = {
	id: ObjectId;
	name?: string;
	parentId?: string;
	prefab?: true;
	position: Vec3Tuple;
	rotation: Vec4Tuple;
	scale: Vec3Tuple;
	geometry: Geometry;
	material: Material;
	gltfModel?: GltfModel | null | undefined;
	splat?: Splat | null | undefined;
	collider?: Collider | null | undefined;
	audio?: AudioSettings | null | undefined;
	videoControls?: VideoControlsGraphSettings | null | undefined;
	ui?: UiGraphSettings | null | undefined;
	hidden?: boolean;
	shadow?: Shadow;
	light?: Light;
	camera?: Camera;
	face?: Face;
	imageTarget?: ImageTarget;
	map?: Map$1;
	mapTheme?: MapTheme;
	mapPoint?: MapPoint;
	components: Record<GraphComponent["id"], GraphComponent>;
	ephemeral?: boolean;
	disabled?: true;
	persistent?: true;
	order?: number;
};
type PrefabInstanceDeletions = Partial<{
	[K in keyof Omit<BaseGraphObject, "id" | "name" | "parentId" | "prefab" | "components">]: true;
}> & Partial<{
	components: Record<GraphComponent["id"], true>;
}>;
type PrefabInstanceChildrenData = Omit<Partial<BaseGraphObject>, "id" | "prefab"> & {
	id?: string;
	deletions?: PrefabInstanceDeletions;
	deleted?: true;
};
type PrefabInstanceChildren = Record<string, PrefabInstanceChildrenData>;
type InstanceData = {
	instanceOf: string;
	deletions: PrefabInstanceDeletions;
	children?: PrefabInstanceChildren;
};
type GraphObject = Partial<BaseGraphObject> & {
	id: ObjectId;
	components: Record<GraphComponent["id"], GraphComponent>;
	instanceData?: InstanceData;
};
type Space = {
	id: string;
	name: string;
	sky?: Sky;
	activeCamera?: string;
	includedSpaces?: string[];
	reflections?: string | Resource | null;
	fog?: Fog;
};
type Spaces = Record<string, Space>;
type Binding = {
	input: string;
	modifiers: string[];
};
type Action = {
	name: string;
	bindings: Binding[];
};
type InputMap = Record<string, Action[]>;
type Color = {
	type: "color";
	color?: string;
};
type GradientStyle = "linear" | "radial";
type Gradient = {
	type: "gradient";
	style?: GradientStyle;
	colors?: string[];
};
type Image$1<T = Resource> = {
	type: "image";
	src?: T;
};
type NoSky = {
	type: "none";
};
type Sky<T = Resource> = Color | Gradient | Image$1<T> | NoSky;
type NoFog = {
	type: "none";
};
type LinearFog = {
	type: "linear";
	near: number;
	far: number;
	color: string;
};
type ExponentialFog = {
	type: "exponential";
	density: number;
	color: string;
};
type Fog = NoFog | LinearFog | ExponentialFog;
type SceneGraph = {
	activeCamera?: string;
	activeMap?: string;
	inputs?: InputMap;
	sky?: Sky;
	reflections?: string | Resource;
	entrySpaceId?: string;
	spaces?: Spaces;
	objects: Record<string, GraphObject>;
	runtimeVersion?: RuntimeVersionTarget;
};
export type Eid = bigint;
type TypeToValue = {
	"eid": Eid;
	"f32": number;
	"f64": number;
	"i32": number;
	"ui8": number;
	"ui32": number;
	"string": string;
	"boolean": boolean;
};
type Type = keyof TypeToValue;
type ElementOf<T extends Type> = TypeToValue[T];
export interface Schema {
	[key: string]: Type;
}
export type ReadData<T extends Schema> = {
	readonly [key in keyof T]: ElementOf<T[key]>;
};
export type WriteData<T extends Schema> = {
	-readonly [key in keyof T]: ElementOf<T[key]>;
};
type OrderedSchema = Array<[
	string,
	Type,
	number
]>;
type SpaceData = {
	id: string;
	name: string;
	spawned: boolean;
};
type SpacesHandle = {
	loadSpace: (idOrName: string) => void;
	listSpaces: () => SpaceData[] | undefined;
	getActiveSpace: () => SpaceData | undefined;
};
type PrefabsHandle = {
	graphIdToPrefab: Map<string, Eid>;
	getPrefab: (name: string) => Eid | undefined;
};
type SceneHandle = SpacesHandle & PrefabsHandle & {
	remove: () => void;
	updateBaseObjects: (newObjects: DeepReadonly<Record<string, GraphObject>>) => void;
	updateDebug: (newGraph: DeepReadonly<SceneGraph>) => void;
	graphIdToEid: Map<string, Eid>;
	eidToObject: Map<Eid, DeepReadonly<GraphObject>>;
	graphIdToPrefab: Map<string, Eid>;
	_graphIdToEidOrPrefab: Map<string, Eid>;
};
type TimeoutId = number;
type Callback = () => void;
type TimeState = {
	elapsed: number;
	delta: number;
	absolute: number;
	absoluteDelta: number;
};
type TimeApi = {
	setTimeout: (cb: Callback, timeout: number) => TimeoutId;
	setInterval: (cb: Callback, timeout: number) => TimeoutId;
	clearTimeout: (id: TimeoutId) => void;
};
type Time = TimeState & TimeApi;
type AudioControls = {
	mute: () => void;
	unmute: () => void;
	pause: () => void;
	play: () => void;
	setVolume: (newVolume: number) => void;
};
interface Vec3Source {
	/**
	 * Access the x component of the vector.
	 */
	readonly x: number;
	/**
	 * Access the y component of the vector.
	 */
	readonly y: number;
	/**
	 * Access the z component of the vector.
	 */
	readonly z: number;
}
interface Vec3 extends Vec3Source {
	/**
	 * Create a new vector with the same components as this vector.
	 *
	 * API Type: Immutable API.
	 *
	 * @returns a new vector with the same components as this vector.
	 */
	clone: () => Vec3;
	/**
	 * Compute the cross product of this vector and another vector.
	 *
	 * API Type: Immutable API.
	 *
	 * @param v vector to compute the cross product with.
	 * @returns the cross product of this vector and another vector.
	 */
	cross: (v: Vec3) => Vec3;
	/**
	 * Access the vector as a homogeneous array (4 dimensions).
	 *
	 * API Type: Immutable API.
	 *
	 * @returns a homogeneous array (4 dimensions) representing the vector.
	 */
	data: () => number[];
	/**
	 * Compute the euclidean distance between this vector and another vector.
	 *
	 * API Type: Immutable API.
	 *
	 * @param v vector to compute the distance to.
	 * @returns the euclidean distance between this vector and v.
	 */
	distanceTo: (v: Vec3Source) => number;
	/**
	 * Element-wise vector division.
	 *
	 * API Type: Immutable API.
	 *
	 * @param v vector to divide by.
	 * @returns the result of dividing each element of this vector by each element of v.
	 */
	divide: (v: Vec3Source) => Vec3;
	/**
	 * Compute the dot product of this vector and another vector.
	 *
	 * API Type: Immutable API.
	 *
	 * @param v vector to compute the dot product with.
	 * @returns the dot product of this vector and v.
	 */
	dot: (v: Vec3Source) => number;
	/**
	 * Check whether two vectors are equal, with a specified floating point tolerance.
	 *
	 * API Type: Immutable API.
	 *
	 * @param v vector to compare to.
	 * @param tolerance used to judge near equality.
	 * @returns true if vector components are each equal within the specified tolerance, false
	 *   otherwise.
	 */
	equals: (v: Vec3Source, tolerance: number) => boolean;
	/**
	 * Compute the length of the vector.
	 *
	 * API Type: Immutable API.
	 *
	 * @returns the length of the vector.
	 */
	length: () => number;
	/**
	 * Subtract a vector from this vector.
	 *
	 * API Type: Immutable API.
	 *
	 * @param v vector to subtract.
	 * @returns the result of subtracting v from this vector.
	 */
	minus: (v: Vec3Source) => Vec3;
	/**
	 * Compute a linear interpolation between this vector and another vector v with a factor t such
	 * that the result is thisVec * (1 - t) + v * t.
	 *
	 * API Type: Immutable API.
	 *
	 * @param v vector to interpolate with.
	 * @param t factor to interpolate; should be between in 0 to 1, inclusive.
	 * @returns the result of the linear interpolation.
	 */
	mix: (v: Vec3Source, t: number) => Vec3;
	/**
	 * Return a new vector with the same direction as this vector, but with a length of 1.
	 *
	 * API Type: Immutable API.
	 *
	 * @returns a new vector with the same direction as this vector, but with a length of 1.
	 */
	normalize: () => Vec3;
	/**
	 * Add two vectors together.
	 *
	 * API Type: Immutable API.
	 *
	 * @param v vector to add.
	 * @returns the result of adding v to this vector.
	 */
	plus: (v: Vec3Source) => Vec3;
	/**
	 * Multiply the vector by a scalar.
	 *
	 * API Type: Immutable API.
	 *
	 * @param s scalar to multiply by.
	 * @returns the result of multiplying this vector by s.
	 */
	scale: (s: number) => Vec3;
	/**
	 * Element-wise vector multiplication.
	 *
	 * API Type: Immutable API.
	 *
	 * @param v vector to multiply by.
	 * @returns the result of multiplying each element of this vector by each element of v.
	 */
	times: (v: Vec3Source) => Vec3;
	/**
	 * Compute the cross product of this vector and another vector. Store the result in this Vec3
	 * and return this Vec3 for chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @param v vector to compute cross product with.
	 * @returns this vector for chaining.
	 */
	setCross: (v: Vec3Source) => Vec3;
	/**
	 * Element-wise vector division. Store the result in this Vec3 and return this Vec3 for
	 * chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @param v vector to divide by.
	 * @returns this vector for chaining.
	 */
	setDivide: (v: Vec3Source) => Vec3;
	/**
	 * Subtract a vector from this vector. Store the result in this Vec3 and return this Vec3
	 * for chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @param v vector to subtract.
	 * @returns this vector for chaining.
	 */
	setMinus: (v: Vec3Source) => Vec3;
	/**
	 * Compute a linear interpolation between this vector and another vector v with a factor t such
	 * that the result is thisVec * (1 - t) + v * t. The factor t should be between 0 and 1. Store the
	 * result in this Vec3 and return this Vec3 for chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @param v vector to interpolate with.
	 * @param t factor to interpolate; should be between in 0 to 1, inclusive.
	 * @returns this vector for chaining.
	 */
	setMix: (v: Vec3Source, t: number) => Vec3;
	/**
	 * Set the vector to be a version of itself with the same direction, but with length 1. Store the
	 * result in this Vec3 and return this Vec3 for chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @returns this vector for chaining.
	 */
	setNormalize: () => Vec3;
	/**
	 * Add two vectors together. Store the result in this Vec3 and return this Vec3 for chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @param v vector to add.
	 * @returns this vector for chaining.
	 */
	setPlus: (v: Vec3Source) => Vec3;
	/**
	 * Multiply the vector by a scalar. Store the result in this Vec3 and return this Vec3 for
	 * chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @param s scalar to multiply by.
	 * @returns this vector for chaining.
	 */
	setScale: (s: number) => Vec3;
	/**
	 * Element-wise vector multiplication. Store the result in this Vec3 and return this Vec3
	 * for chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @param v vector to multiply by.
	 * @returns this vector for chaining.
	 */
	setTimes: (v: Vec3Source) => Vec3;
	/**
	 * Set the Vec3's x component. Store the result in this Vec3 and return this for chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @param v value to set this vector's x component to.
	 * @returns this vector for chaining.
	 */
	setX(v: number): Vec3;
	/**
	 * Set the Vec3's y component. Store the result in this Vec3 and return this Vec3 for chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @param v value to set this vector's y component to.
	 * @returns this vector for chaining.
	 */
	setY(v: number): Vec3;
	/**
	 * Set the Vec3's z component. Store the result in this Vec3 and return this Vec3 for chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @param v value to set this vector's z component to.
	 * @returns this vector for chaining.
	 */
	setZ(v: number): Vec3;
	/**
	 * Set the Vec3 to be all ones. Store the result in this Vec3 and return this Vec3 for chaining.
	 *
	 * API Type: Set API.
	 *
	 * @returns this vector for chaining.
	 */
	makeOne: () => Vec3;
	/**
	 * Set the Vec3 to have all components set to the scale value s. Store the result in this Vec3
	 * and return this Vec3 for chaining.
	 *
	 * API Type: Set API.
	 *
	 * @param s value to set all components to.
	 * @returns this vector for chaining.
	 */
	makeScale: (s: number) => Vec3;
	/**
	 * Set the Vec3 to be pointed in the positive y direction. Store the result in this Vec3 and
	 * return this Vec3 for chaining.
	 *
	 * API Type: Set API.
	 *
	 * @returns this vector for chaining.
	 */
	makeUp: () => Vec3;
	/**
	 * Set the Vec3 to be all zeros. Store the result in this Vec3 and return this Vec3 for
	 * chaining.
	 *
	 * API Type: Set API.
	 *
	 * @returns this vector for chaining.
	 */
	makeZero: () => Vec3;
	/**
	 * Set this Vec3 to have the same value as another Vec3. Store the result in this Vec3 and return
	 * this Vec3 for chaining.
	 *
	 * API Type: Set API.
	 *
	 * @param v vector to copy from.
	 * @returns this vector for chaining.
	 */
	setFrom: (v: Vec3Source) => Vec3;
	/**
	 * Set the Vec3's x, y, and z components. Store the result in this Vec3 and return this for
	 * chaining.
	 *
	 * API Type: Set API.
	 *
	 * @param x value to set this vector's x component to.
	 * @param y value to set this vector's y component to.
	 * @param z value to set this vector's z component to.
	 * @returns this vector for chaining.
	 */
	setXyz: (x: number, y: number, z: number) => Vec3;
}
interface Vec3Factory {
	/**
	 * Create a Vec3 from an object with x, y, z properties.
	 *
	 * API Type: Factory API.
	 *
	 * @param source to copy.
	 * @returns a new Vec3 with the same components as the source.
	 */
	from: (source: Vec3Source) => Vec3;
	/**
	 * Create a Vec3 with all elements set to one.
	 *
	 * API Type: Factory API.
	 *
	 * @returns a new Vec3 with all elements set to one.
	 */
	one: () => Vec3;
	/**
	 * Create a Vec3 with all elements set to the scale value s.
	 *
	 * API Type: Factory API.
	 *
	 * @param s value to set all components to.
	 * @returns a new Vec3 with all elements set to the scale value s.
	 */
	scale: (s: number) => Vec3;
	/**
	 * Create a Vec3 pointing in the positive y direction.
	 *
	 * API Type: Factory API.
	 *
	 * @returns a new Vec3 pointing in the positive y direction.
	 */
	up: () => Vec3;
	/**
	 * Create a Vec3 from x, y, z values.
	 *
	 * API Type: Factory API.
	 *
	 * @param x value to set the x component to.
	 * @param y value to set the y component to.
	 * @param z value to set the z component to.
	 * @returns a new Vec3 with the x, y, and z components set to the specified values.
	 */
	xyz: (x: number, y: number, z: number) => Vec3;
	/**
	 * Create a Vec3 with all components set to zero.
	 *
	 * API Type: Factory API.
	 *
	 * @returns a new Vec3 with all components set to zero.
	 */
	zero: () => Vec3;
}
declare const vec3: Vec3Factory;
interface QuatSource {
	/**
	 * Access the x component of the quaternion.
	 */
	readonly x: number;
	/**
	 * Access the y component of the quaternion.
	 */
	readonly y: number;
	/**
	 * Access the z component of the quaternion.
	 */
	readonly z: number;
	/**
	 * Access the w component of the quaternion.
	 */
	readonly w: number;
}
interface Quat extends QuatSource {
	/**
	 * Convert the quaternion to an axis-angle representation.  The direction of the vector gives the
	 * axis of rotation, and the magnitude of the vector gives the angle, in radians. If `target` is
	 * supplied, the result will be stored in `target` and `target` will be returned. Otherwise, a new
	 * Vec3 will be created and returned.
	 *
	 * API Type: Immutable API.
	 *
	 * @param target optional vector to store the result in.
	 * @returns target if supplied, otherwise a new Vec3.
	 */
	axisAngle: (target?: Vec3) => Vec3;
	/**
	 * Create a new quaternion with the same components as this quaternion.
	 *
	 * API Type: Immutable API.
	 *
	 * @returns a new quaternion with the same components as this quaternion.
	 */
	clone: () => Quat;
	/**
	 * Return the rotational conjugate of this quaternion. The conjugate of a quaternion represents
	 * the same rotation in the opposite direction about the rotational axis.
	 *
	 * API Type: Immutable API.
	 *
	 * @returns a new quaternion representing the rotational conjugate of this quaternion.
	 */
	conjugate: () => Quat;
	/**
	 * Access the quaternion as an array of [x, y, z, w].
	 *
	 * API Type: Immutable API.
	 *
	 * @returns an array of [x, y, z, w].
	 */
	data: () => number[];
	/**
	 * Angle between two quaternions, in degrees.
	 *
	 * API Type: Immutable API.
	 *
	 * @param target quaternion to compute the angle to.
	 * @returns the angle between this quaternion and the target quaternion, in degrees.
	 */
	degreesTo: (target: QuatSource) => number;
	/**
	 * Compute the quaternion required to rotate this quaternion to the target quaternion.
	 *
	 * API Type: Immutable API.
	 *
	 * @param target quaternion to rotate towards.
	 * @returns the quaternion required to rotate this quaternion to the target quaternion.
	 */
	delta: (target: QuatSource) => Quat;
	/**
	 * Compute the dot product of this quaternion with another quaternion.
	 *
	 * API Type: Immutable API.
	 *
	 * @param q quaternion to compute the dot product with.
	 * @returns the dot product of this quaternion with the target quaternion.
	 */
	dot: (q: QuatSource) => number;
	/**
	 * Check whether two quaternions are equal, with a specified floating point tolerance.
	 *
	 * API Type: Immutable API.
	 *
	 * @param v quaternion to compare to.
	 * @param tolerance used to judge near equality.
	 * @returns true if quaternions components are each equal within the specified tolerance, false
	 *   otherwise.
	 */
	equals: (q: QuatSource, tolerance: number) => boolean;
	/**
	 * Compute the quaternion which multiplies this quaternion to get a zero rotation quaternion.
	 *
	 * API Type: Immutable API.
	 *
	 * @returns the inverse of this quaternion.
	 */
	inv: () => Quat;
	/**
	 * Negate all components of this quaternion. The result is a quaternion representing the same
	 * rotation as this quaternion.
	 *
	 * API Type: Immutable API.
	 *
	 * @returns the negated quaternion.
	 */
	negate: () => Quat;
	/**
	 * Get the normalized version of this quaternion with a length of 1.
	 *
	 * API Type: Immutable API.
	 *
	 * @returns the normalized quaternion.
	 */
	normalize: () => Quat;
	/**
	 * Convert the quaternion to pitch, yaw, and roll angles in radians.
	 *
	 * API Type: Immutable API.
	 *
	 * @param target optional vector to store the result in.
	 * @returns target if supplied, otherwise a new Vec3.
	 */
	pitchYawRollRadians: (target?: Vec3) => Vec3;
	/**
	 * Convert the quaternion to pitch, yaw, and roll angles in degrees.
	 *
	 * API Type: Immutable API.
	 *
	 * @param target optional vector to store the result in.
	 * @returns target if supplied, otherwise a new Vec3.
	 */
	pitchYawRollDegrees: (target?: Vec3) => Vec3;
	/**
	 * Add two quaternions together.
	 *
	 * API Type: Immutable API.
	 *
	 * @param q quaternion to add.
	 * @returns the sum of this quaternion and the target quaternion.
	 */
	plus: (q: QuatSource) => Quat;
	/**
	 * Angle between two quaternions, in radians.
	 *
	 * API Type: Immutable API.
	 *
	 * @param target quaternion to compute the angle to.
	 * @returns the angle between this quaternion and the target quaternion, in radians.
	 */
	radiansTo: (target: QuatSource) => number;
	/**
	 * Rotate this quaternion towards the target quaternion by a given number of radians, clamped to
	 * the target.
	 *
	 * API Type: Immutable API.
	 *
	 * @param target quaternion to rotate towards.
	 * @param radians number of radians to rotate.
	 * @returns the rotated quaternion.
	 */
	rotateToward: (target: QuatSource, radians: number) => Quat;
	/**
	 * Spherical interpolation between two quaternions given a provided interpolation value. If the
	 * interpolation is set to 0, then it will return this quaternion. If the interpolation is set to
	 * 1, then it will return the target quaternion.
	 *
	 * API Type: Immutable API.
	 *
	 * @param target quaternion to interpolate towards.
	 * @param t factor to interpolate; should be between in 0 to 1, inclusive.
	 * @returns the interpolated quaternion.
	 */
	slerp: (target: QuatSource, t: number) => Quat;
	/**
	 * Multiply two quaternions together.
	 *
	 * API Type: Immutable API.
	 *
	 * @param q quaternion to multiply.
	 * @returns the product of this quaternion and the target quaternion.
	 */
	times: (q: QuatSource) => Quat;
	/**
	 * Multiply the quaternion by a vector. This is equivalent to converting the quaternion to a
	 * rotation matrix and multiplying the matrix by the vector.
	 *
	 * API Type: Immutable API.
	 *
	 * @param v vector to multiply.
	 * @param target optional vector to store the result in.
	 * @returns target if supplied, otherwise a new Vec3.
	 */
	timesVec: (v: Vec3Source, target?: Vec3) => Vec3;
	/**
	 * Set this quaternion to its rotational conjugate. The conjugate of a quaternion represents the
	 * same rotation in the opposite direction about the rotational axis. Store the result in this
	 * Quat and return this Quat for chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @returns this quaternion for chaining.
	 */
	setConjugate: () => Quat;
	/**
	 * Compute the quaternion required to rotate this quaternion to the target quaternion. Store the
	 * result in this Quat and return this Quat for chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @param target quaternion to rotate towards.
	 * @returns this quaternion for chaining.
	 */
	setDelta: (target: QuatSource) => Quat;
	/**
	 * Set this quaternion to the value in another quaternion. Store the result in this Quat and
	 * return this Quat for chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @param q quaternion to set from.
	 * @returns this quaternion for chaining.
	 */
	setFrom: (q: QuatSource) => Quat;
	/**
	 * Set this to the quaternion which multiplies this quaternion to get a zero rotation quaternion.
	 * Store the result in this Quat and return this Quat for chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @returns this quaternion for chaining.
	 */
	setInv: () => Quat;
	/**
	 * Negate all components of this quaternion. The result is a quaternion representing the same
	 * rotation as this quaternion. Store the result in this Quat and return this Quat for chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @returns this quaternion for chaining.
	 */
	setNegate: () => Quat;
	/**
	 * Get the normalized version of this quaternion with a length of 1. Store the result in this
	 * Quat and return this Quat for chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @returns this quaternion for chaining.
	 */
	setNormalize: () => Quat;
	/**
	 * Add this quaternion to another quaternion. Store the result in this Quat and return this
	 * Quat for chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @param q quaternion to add.
	 * @returns this quaternion for chaining.
	 */
	setPlus: (q: QuatSource) => Quat;
	/**
	 * Set this quaternion to the result of q times this quaternion. Store the result in this Quat
	 * and return this Quat for chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @param q quaternion to premultiply.
	 * @returns this quaternion for chaining.
	 */
	setPremultiply: (q: QuatSource) => Quat;
	/**
	 * Rotate this quaternion towards the target quaternion by a given number of radians, clamped to
	 * the target. Store the result in this Quat and return this Quat for chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @param target quaternion to rotate towards.
	 * @param radians number of radians to rotate.
	 * @returns this quaternion for chaining.
	 */
	setRotateToward: (target: QuatSource, radians: number) => Quat;
	/**
	 * Spherical interpolation between two quaternions given a provided interpolation value. If the
	 * interpolation is set to 0, then it will return this quaternion. If the interpolation is set to
	 * 1, then it will return the target quaternion. Store the result in this Quat and return this
	 * Quat for chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @param target quaternion to interpolate towards.
	 * @param t factor to interpolate; should be between in 0 to 1, inclusive.
	 * @returns this quaternion for chaining.
	 */
	setSlerp: (target: QuatSource, t: number) => Quat;
	/**
	 * Multiply two quaternions together. Store the result in this Quat and return this Quat for
	 * chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @param q quaternion to multiply.
	 * @returns this quaternion for chaining.
	 */
	setTimes: (q: QuatSource) => Quat;
	/**
	 * Set the quaternion to the specified x, y, z and w values. Store the result in this Quat and
	 * return this Quat for chaining.
	 *
	 * API Type: Set API.
	 *
	 * @param x x component of the quaternion.
	 * @param y y component of the quaternion.
	 * @param z z component of the quaternion.
	 * @param w w component of the quaternion.
	 * @returns this quaternion for chaining.
	 */
	setXyzw: (x: number, y: number, z: number, w: number) => Quat;
	/**
	 * Set a Quat from an axis-angle representation. The direction of the vector gives the axis of
	 * rotation, and the magnitude of the vector gives the angle, in radians. Store the result in this
	 * Quat and return this Quat for chaining.
	 *
	 * API Type: Set API.
	 *
	 * @param aa vector containing the axis-angle representation of the rotation.
	 * @returns this quaternion for chaining.
	 */
	makeAxisAngle: (aa: Vec3Source) => Quat;
	/**
	 * Set the quaternion to a rotation specified by pitch, yaw, and roll angles in radians. Store the
	 * result in this Quat and return this Quat for chaining.
	 *
	 * API Type: Set API.
	 *
	 * @param v vector containing the pitch, yaw, and roll angles in radians.
	 * @returns this quaternion for chaining.
	 */
	makePitchYawRollRadians: (v: Vec3Source) => Quat;
	/**
	 * Set the quaternion to a rotation that would cause the eye to look at the target with the given
	 * up vector. Store the result in this Quat and return this Quat for chaining.
	 *
	 * API Type: Set API.
	 *
	 * @param eye vector where the eye is located.
	 * @param target vector where the target is located.
	 * @param up vector representing the up direction from the eye's perspective.
	 * @returns this quaternion for chaining.
	 */
	makeLookAt: (eye: Vec3Source, target: Vec3Source, up: Vec3Source) => Quat;
	/**
	 * Set the quaternion to a rotation specified by pitch, yaw, and roll angles in degrees. Store the
	 * result in this Quat and return this Quat for chaining.
	 *
	 * API Type: Set API.
	 *
	 * @param v vector containing the pitch, yaw, and roll angles in degrees.
	 * @returns this quaternion for chaining.
	 */
	makePitchYawRollDegrees: (v: Vec3Source) => Quat;
	/**
	 * Set the quaternion to a rotation about the x-axis (pitch) in degrees. Store the result in this
	 * Quat and return this Quat for chaining.
	 *
	 * API Type: Set API.
	 *
	 * @param degrees the angle of rotation in degrees.
	 * @returns this quaternion for chaining.
	 */
	makeXDegrees: (degrees: number) => Quat;
	/**
	 * Set the quaternion to a rotation about the x-axis (pitch) in radians. Store the result in this
	 * Quat and return this Quat for chaining.
	 *
	 * API Type: Set API.
	 *
	 * @param radians the angle of rotation in radians.
	 * @returns this quaternion for chaining.
	 */
	makeXRadians: (radians: number) => Quat;
	/**
	 * Set the quaternion to a rotation about the y-axis (yaw) in degrees. Store the result in this
	 * Quat and return this Quat for chaining.
	 *
	 * API Type: Set API.
	 *
	 * @param degrees the angle of rotation in degrees.
	 * @returns this quaternion for chaining.
	 */
	makeYDegrees: (degrees: number) => Quat;
	/**
	 * Set the quaternion to a rotation about the y-axis (yaw) in radians. Store the result in this
	 * Quat and return this Quat for chaining.
	 *
	 * API Type: Set API.
	 *
	 * @param radians the angle of rotation in radians.
	 * @returns this quaternion for chaining.
	 */
	makeYRadians: (radians: number) => Quat;
	/**
	 * Set the quaternion to a rotation about the z-axis (roll) in degrees. Store the result in this
	 * Quat and return this Quat for chaining.
	 *
	 * API Type: Set API.
	 *
	 * @param degrees the angle of rotation in degrees.
	 * @returns this quaternion for chaining.
	 */
	makeZDegrees: (degrees: number) => Quat;
	/**
	 * Set the quaternion to a rotation about the z-axis (roll) in radians. Store the result in this
	 * Quat and return this Quat for chaining.
	 *
	 * API Type: Set API.
	 *
	 * @param radians the angle of rotation in radians.
	 * @returns this quaternion for chaining.
	 */
	makeZRadians: (radians: number) => Quat;
	/**
	 * Set the quaternion to a zero rotation. Store the result in this Quat and return this Quat for
	 * chaining.
	 *
	 * API Type: Set API.
	 *
	 * @returns this quaternion for chaining.
	 */
	makeZero: () => Quat;
}
interface QuatFactory {
	/**
	 * Create a Quat from an axis-angle representation. The direction of the `aa` vector gives the
	 * axis of rotation, and the magnitude of the vector gives the angle, in radians. For example,
	 * `quat.axisAngle(vec3.up().scale(Math.PI / 2))` represents a 90-degree rotation about the
	 * y-axis, and is equivalent to `quat.yDegrees(90)`. If `target` is supplied, the result will be
	 * stored in `target` and `target` will be returned. Otherwise, a new Quat will be created and
	 * returned.
	 *
	 * API Type: Factory API.
	 *
	 * @param aa vector containing the axis-angle representation of the rotation.
	 * @param target optional quaternion to store the result in.
	 * @returns target if supplied, otherwise a new Quat.
	 */
	axisAngle: (aa: Vec3Source, target?: Quat) => Quat;
	/**
	 * Create a Quat from an object with x, y, z, w properties.
	 *
	 * API Type: Factory API.
	 *
	 * @param source object with x, y, z, w properties.
	 * @returns a new Quat with the same components as the source object.
	 */
	from: (source: QuatSource) => Quat;
	/**
	 * Create a Quat representing the rotation required for an object positioned at `eye` to look at
	 * an object positioned at `target`, with the given `up` vector.
	 *
	 * API Type: Factory API.
	 *
	 * @param eye vector where the eye is located.
	 * @param target vector where the target is located.
	 * @param up vector representing the up direction from the eye's perspective.
	 * @returns a new Quat representing the rotation required for an object positioned at `eye` to
	 *  look at an object positioned at `target`, with the given `up` vector.
	 */
	lookAt: (eye: Vec3Source, target: Vec3Source, up: Vec3Source) => Quat;
	/**
	 * Construct a quaternion from a pitch / yaw / roll representation, also known as YXZ Euler
	 * angles. Rotation is specified in degrees.
	 *
	 * API Type: Factory API.
	 *
	 * @param v vector containing the pitch, yaw, and roll angles in degrees.
	 * @returns a new Quat representing the rotation specified by the pitch, yaw, and roll angles in
	 *   degrees.
	 */
	pitchYawRollDegrees: (v: Vec3Source) => Quat;
	/**
	 * Construct a quaternion from a pitch / yaw / roll representation, also known as YXZ Euler
	 * angles. Rotation is specified in radians.
	 *
	 * API Type: Factory API.
	 *
	 * @param v rotation specified in radians.
	 * @returns a new Quat representing the rotation specified by the pitch, yaw, and roll angles in
	 *   radians.
	 */
	pitchYawRollRadians: (v: Vec3Source) => Quat;
	/**
	 * Create a Quat which represents a rotation about the x-axis. Rotation is specified in degrees.
	 *
	 * API Type: Factory API.
	 *
	 * @param degrees to rotate.
	 * @returns a new Quat representing the rotation.
	 */
	xDegrees: (degrees: number) => Quat;
	/**
	 * Create a Quat which represents a rotation about the x-axis. Rotation is specified in radians.
	 *
	 * API Type: Factory API.
	 *
	 * @param radians to rotate.
	 * @returns a new Quat representing the rotation.
	 */
	xRadians: (radians: number) => Quat;
	/**
	 * Create a Quat from the specified x, y, z, and w values.
	 *
	 * API Type: Factory API.
	 *
	 * @param x component of the quaternion.
	 * @param y component of the quaternion.
	 * @param z component of the quaternion.
	 * @param w component of the quaternion.
	 * @returns a new Quat with the specified components.
	 */
	xyzw: (x: number, y: number, z: number, w: number) => Quat;
	/**
	 * Create a Quat which represents a rotation about the y-axis. Rotation is specified in degrees.
	 *
	 * API Type: Factory API.
	 *
	 * @param degrees to rotate.
	 * @returns a new Quat representing the rotation.
	 */
	yDegrees: (degrees: number) => Quat;
	/**
	 * Create a Quat which represents a rotation about the y-axis. Rotation is specified in radians.
	 *
	 * API Type: Factory API.
	 *
	 * @param radians to rotate.
	 * @returns a new Quat representing the rotation.
	 */
	yRadians: (radians: number) => Quat;
	/**
	 * Create a Quat which represents a rotation about the z-axis. Rotation is specified in degrees.
	 *
	 * API Type: Factory API.
	 *
	 * @param degrees to rotate.
	 * @returns a new Quat representing the rotation.
	 */
	zDegrees: (degrees: number) => Quat;
	/**
	 * Create a Quat which represents a rotation about the z-axis. Rotation is specified in radians.
	 *
	 * API Type: Factory API.
	 *
	 * @param radians to rotate.
	 * @returns a new Quat representing the rotation.
	 */
	zRadians: (radians: number) => Quat;
	/**
	 * Create a Quat which represents a zero rotation.
	 *
	 * API Type: Factory API.
	 *
	 * @returns a new Quat representing a zero rotation.
	 */
	zero: () => Quat;
}
declare const quat: QuatFactory;
interface Mat4 {
	/**
	 * Create a new matrix with the same components as this matrix.
	 *
	 * API Type: Immutable API.
	 *
	 * @returns a new matrix with the same components as this matrix.
	 */
	clone: () => Mat4;
	/**
	 * Get the raw data of the matrix, in column-major order.
	 *
	 * API Type: Immutable API.
	 *
	 * @returns the 16-element raw data of the matrix, in column-major order.
	 */
	data: () => number[];
	/**
	 * Decompose the matrix into its translation, rotation, and scale components, assuming it was
	 * formed by a translation, rotation, and scale in that order. If `target` is supplied, the result
	 * will be stored in `target` and `target` will be returned. Otherwise, a new {t, r, s} object
	 * will be created and returned.
	 *
	 * If you don't need the rotation in quaternion form, decomposeT and decomposeS are more efficient
	 * ways to get just the translation or scale.
	 *
	 * API Type: Immutable API.
	 *
	 * @param target optional target object to store the result in.
	 * @returns target if supplied, otherwise a new {t, r, s} object.
	 */
	decomposeTrs: (target?: {
		t: Vec3;
		r: Quat;
		s: Vec3;
	}) => {
		t: Vec3;
		r: Quat;
		s: Vec3;
	};
	/**
	 * Get the rotation component of the TRS matrix
	 *
	 * @param target optional target object to store the result in.
	 * @returns target if supplied, otherwise a new quaternion.
	 */
	decomposeR: (target?: Quat) => Quat;
	/**
	 * Get the translation component of the TRS matrix.
	 *
	 * @param target optional target object to store the result in.
	 * @returns target if supplied, otherwise a new vec3.
	 */
	decomposeT: (target?: Vec3) => Vec3;
	/**
	 * Get the scale component of the TRS matrix.
	 *
	 * @param target optional target object to store the result in.
	 * @returns target if supplied, otherwise a new vec3.
	 */
	decomposeS: (target?: Vec3) => Vec3;
	/**
	 * Compute the determinant of the matrix.
	 *
	 * API Type: Immutable API.
	 *
	 * @returns the determinant of the matrix.
	 */
	determinant: () => number;
	/**
	 * Check whether two matrices are equal, with a specified floating point tolerance.
	 *
	 * API Type: Immutable API.
	 *
	 * @param m matrix to compare to.
	 * @param tolerance used to judge near equality.
	 * @returns true if all matrix elements are equal within the specified tolerance, false otherwise.
	 */
	equals: (m: Mat4, tolerance: number) => boolean;
	/**
	 * Invert the matrix, or throw if the matrix is not invertible. Because Mat4 stores a precomputed
	 * inverse, this operation is very fast.
	 *
	 * API Type: Immutable API.
	 *
	 * @returns the inverse of the matrix.
	 */
	inv: () => Mat4;
	/**
	 * Get the raw data of the inverse matrix, in column-major order, or null if the matrix is not
	 * invertible.
	 *
	 * API Type: Immutable API.
	 *
	 * @returns the 16-element raw data of the inverse matrix, in column-major order, or null if the
	 *   matrix is not invertible.
	 */
	inverseData: () => number[] | null;
	/**
	 * Get a matrix with the same position and scale as this matrix, but with the rotation set to look
	 * at the target.
	 *
	 * API Type: Immutable API.
	 *
	 * @param target vector where the target is located.
	 * @param up vector representing the up direction from the mat4's perspective.
	 * @returns a new matrix with the same position and scale as this matrix, but with the rotation
	 *   set to look at the target.
	 */
	lookAt: (target: Vec3Source, up: Vec3Source) => Mat4;
	/**
	 * Multiply the matrix by a scalar. Scaling by 0 throws an error.
	 *
	 * API Type: Immutable API.
	 *
	 * @param s scalar to multiply the matrix by.
	 * @returns the matrix multiplied by the scalar.
	 */
	scale: (s: number) => Mat4;
	/**
	 * Get the transpose of the matrix.
	 *
	 * API Type: Immutable API.
	 *
	 * @returns the transpose of the matrix.
	 */
	transpose: () => Mat4;
	/**
	 * Multiply the matrix by another matrix.
	 *
	 * API Type: Immutable API.
	 *
	 * @param m matrix to multiply by.
	 * @returns the matrix multiplied by another matrix.
	 */
	times: (m: Mat4) => Mat4;
	/**
	 * Multiply the matrix by a vector using homogeneous coordinates.
	 *
	 * API Type: Immutable API.
	 *
	 * @param v vector to multiply by.
	 * @param target optional target to store the result in.
	 * @returns the transformed vector.
	 */
	timesVec: (v: Vec3Source, target?: Vec3) => Vec3;
	/**
	 * Invert the matrix, or throw if the matrix is not invertible. Because Mat4 stores a precomputed
	 * inverse, this operation is very fast. Store the result in this Mat4 and return this Mat4 for
	 * chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @returns this matrix for chaining.
	 */
	setInv: () => Mat4;
	/**
	 * Set the matrix rotation to look at the target, keeping translation and scale unchanged. Store
	 * the result in this Mat4 and return this Mat4 for chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @param target vector where the target is located.
	 * @param up vector representing the up direction from the mat4's perspective.
	 * @returns this matrix for chaining.
	 */
	setLookAt: (target: Vec3Source, up: Vec3Source) => Mat4;
	/**
	 * Set the matrix to the result of m times this matrix. Store the result in this Mat4 and return
	 * this Mat4 for chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @param m matrix to premultiply by.
	 * @returns this matrix for chaining.
	 */
	setPremultiply: (m: Mat4) => Mat4;
	/**
	 * Multiply each element of the matrix by a scaler. Scaling by 0 throws an error. Store the result
	 * in this Mat4 and return this Mat4 for chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @param s scalar to multiply the matrix by.
	 * @returns this matrix for chaining.
	 */
	setScale: (s: number) => Mat4;
	/**
	 * Multiply the matrix by another matrix. Store the result in this Mat4 and return this Mat4 for
	 * chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @param m matrix to multiply by.
	 * @returns this matrix for chaining.
	 */
	setTimes: (m: Mat4) => Mat4;
	/**
	 * Set the matrix to its transpose. Store the result in this Mat4 and return this Mat4 for
	 * chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @returns this matrix for chaining.
	 */
	setTranspose: () => Mat4;
	/**
	 * Set the matrix to the identity matrix. Store the result in this Mat4 and return this Mat4 for
	 * chaining.
	 *
	 * API Type: Set API.
	 *
	 * @returns this matrix for chaining.
	 */
	makeI: () => Mat4;
	/**
	 * Set this matrix to a rotation matrix from the specified quaternion. Store the result in this
	 * Mat4 and return this Mat4 for chaining.
	 *
	 * API Type: Set API.
	 *
	 * @param r quaternion representing the desired rotation matrix.
	 * @returns this matrix for chaining.
	 */
	makeR: (r: QuatSource) => Mat4;
	/**
	 * Create a matrix with specified row data, and optionally specified inverse row data. `dataRows`
	 * and `inverseDataRows` should be four arrays, each with four numbers. If the inverse is not
	 * specified, it will be computed if the matrix is invertible. If the matrix is not invertible,
	 * calling inv() will throw an error.
	 *
	 * API Type: Set API.
	 *
	 * @param rowData for the matrix, 4 arrays of 4 elements each.
	 * @param inverseRowData optional inverse row data for the matrix, 4 arrays of 4 elements each.
	 * @returns this matrix for chaining.
	 */
	makeRows: (rowData: number[][], inverseRowData?: number[][]) => Mat4;
	/**
	 * Set this matrix to a scale matrix from the specified vector. No element of the vector should be
	 * zero. Store the result in this Mat4 and return this Mat4 for chaining.
	 *
	 * API Type: Set API.
	 *
	 * @param s vector representing the desired scale in each of the x, y, and z dimensions.
	 * @returns this matrix for chaining.
	 */
	makeS: (s: Vec3Source) => Mat4;
	/**
	 * Set this matrix to a translation matrix from the specified vector. Store the result in this
	 * Mat4 and return this Mat4 for chaining.
	 *
	 * API Type: Set API.
	 *
	 * @param t vector representing the desired translation in each of the x, y, and z dimensions.
	 * @returns this matrix for chaining.
	 */
	makeT: (t: Vec3Source) => Mat4;
	/**
	 * Set this matrix to a translation and rotation matrix from the specified vector and quaternion.
	 * Store the result in this Mat4 and return this Mat4 for chaining.
	 *
	 * API Type: Set API.
	 *
	 * @param t vector representing the desired translation in each of the x, y, and z dimensions.
	 * @param r quaternion representing the desired rotation matrix.
	 * @returns this matrix for chaining.
	 */
	makeTr: (t: Vec3Source, r: QuatSource) => Mat4;
	/**
	 * Set this matrix to a translation, rotation, and scale matrix from the specified vectors and
	 * quaternion. Store the result in this Mat4 and return this Mat4 for chaining.
	 *
	 * API Type: Set API.
	 *
	 * @param t vector representing the desired translation in each of the x, y, and z dimensions.
	 * @param r quaternion representing the desired rotation matrix.
	 * @param s vector representing the desired scale in each of the x, y, and z dimensions.
	 * @returns this matrix for chaining.
	 */
	makeTrs: (t: Vec3Source, r: QuatSource, s: Vec3Source) => Mat4;
	/**
	 * Set the value of the matrix and inverse to the provided values. If no inverse is provided, one
	 * will be computed if possible. If the matrix is not invertible, calling inv() will throw an
	 * error. Store the result in this Mat4 and return this Mat4 for chaining.
	 *
	 * API Type: Set API.
	 *
	 * @param data for the matrix, 16 elements in column-major order.
	 * @param inverseData optional inverse data for the matrix, 16 elements in column-major order.
	 * @returns this matrix for chaining.
	 */
	set: (data: number[], inverseData?: number[]) => Mat4;
}
interface Mat4Factory {
	/**
	 * Identity matrix.
	 *
	 * API Type: Factory API.
	 *
	 * @returns the identity matrix.
	 */
	i: () => Mat4;
	/**
	 * Create the matrix with directly specified data, in column major order. An optional inverse can
	 * be specified. If the inverse is not specified, it will be computed if the matrix is invertible.
	 * If the matrix is not invertible, calling inv() will throw an error.
	 *
	 * API Type: Factory API.
	 *
	 * @param data for the matrix, 16 elements in column-major order.
	 * @param inverseData optional inverse data for the matrix, 16 elements in column-major order.
	 * @returns the matrix with the specified data.
	 */
	of: (data: number[], inverseData?: number[]) => Mat4;
	/**
	 * Create a rotation matrix from a quaternion.
	 *
	 * API Type: Factory API.
	 *
	 * @param q quaternion representing the rotation.
	 * @returns the rotation matrix.
	 */
	r: (q: QuatSource) => Mat4;
	/**
	 * Create a matrix with specified row data, and optionally specified inverse row data. `dataRows`
	 * and `inverseDataRows` should be four arrays, each with four numbers. If the inverse is not
	 * specified, it will be computed if the matrix is invertible. If the matrix is not invertible,
	 * calling inv() will throw an error.
	 *
	 * API Type: Factory API.
	 *
	 * @param dataRows for the matrix, 4 arrays of 4 elements each.
	 * @param inverseDataRows optional inverse row data for the matrix, 4 arrays of 4 elements each.
	 * @returns the matrix with the specified row data.
	 */
	rows: (dataRows: number[][], inverseDataRows?: number[][]) => Mat4;
	/**
	 * Create a scale matrix. No scale element should be zero.
	 *
	 * API Type: Factory API.
	 *
	 * @param v vector representing the scale in each of the x, y, and z dimensions.
	 * @returns the scale matrix.
	 */
	s: (v: Vec3Source) => Mat4;
	/**
	 * Create a translation matrix.
	 *
	 * API Type: Factory API.
	 *
	 * @param v vector representing the translation in each of the x, y, and z dimensions.
	 * @returns the translation matrix.
	 */
	t: (v: Vec3Source) => Mat4;
	/**
	 * Create a translation and rotation matrix.
	 *
	 * API Type: Factory API.
	 *
	 * @param t vector representing the translation in each of the x, y, and z dimensions.
	 * @param r quaternion representing the rotation matrix.
	 * @returns the translation and rotation matrix.
	 */
	tr: (t: Vec3Source, r: QuatSource) => Mat4;
	/**
	 * Create a translation, rotation, and scale matrix.
	 *
	 * API Type: Factory API.
	 *
	 * @param t vector representing the translation in each of the x, y, and z dimensions.
	 * @param r quaternion representing the rotation matrix.
	 * @param s vector representing the scale in each of the x, y, and z dimensions.
	 * @returns the translation, rotation, and scale matrix.
	 */
	trs: (t: Vec3Source, r: QuatSource, s: Vec3Source) => Mat4;
}
declare const mat4: Mat4Factory;
interface Vec2Source {
	/**
	 * Access the x component of the vector.
	 */
	readonly x: number;
	/**
	 * Access the y component of the vector.
	 */
	readonly y: number;
}
interface Vec2 extends Vec2Source {
	/**
	 * Create a new vector with the same components as this vector.
	 *
	 * API Type: Immutable API.
	 *
	 * @returns a new vector with the same components as this vector.
	 */
	clone: () => Vec2;
	/**
	 * Compute the cross product of this vector and another vector. For 2D vectors, the cross product
	 * is the magnitude of the z component of the 3D cross product of the two vectors with 0 as the z
	 * component.
	 *
	 * API Type: Immutable API.
	 *
	 * @param v vector to compute the cross product with.
	 * @returns the cross product of this vector and another vector.
	 */
	cross: (v: Vec2) => number;
	/**
	 * Compute the euclidean distance between this vector and another vector.
	 *
	 * API Type: Immutable API.
	 *
	 * @param v vector to compute the distance to.
	 * @returns the euclidean distance between this vector and v.
	 */
	distanceTo: (v: Vec2Source) => number;
	/**
	 * Element-wise vector division.
	 *
	 * API Type: Immutable API.
	 *
	 * @param v vector to divide by.
	 * @returns the result of dividing each element of this vector by each element of v.
	 */
	divide: (v: Vec2Source) => Vec2;
	/**
	 * Compute the dot product of this vector and another vector.
	 *
	 * API Type: Immutable API.
	 *
	 * @param v vector to compute the dot product with.
	 * @returns the dot product of this vector and v.
	 */
	dot: (v: Vec2Source) => number;
	/**
	 * Check whether two vectors are equal, with a specified floating point tolerance.
	 *
	 * API Type: Immutable API.
	 *
	 * @param v vector to compare to.
	 * @param tolerance used to judge near equality.
	 * @returns true if vector components are each equal within the specified tolerance, false
	 *   otherwise.
	 */
	equals: (v: Vec2Source, tolerance: number) => boolean;
	/**
	 * Compute the length of the vector.
	 *
	 * API Type: Immutable API.
	 *
	 * @returns the length of the vector.
	 */
	length: () => number;
	/**
	 * Subtract a vector from this vector.
	 *
	 * API Type: Immutable API.
	 *
	 * @param v vector to subtract.
	 * @returns the result of subtracting v from this vector.
	 */
	minus: (v: Vec2Source) => Vec2;
	/**
	 * Compute a linear interpolation between this vector and another vector v with a factor t such
	 * that the result is thisVec * (1 - t) + v * t.
	 *
	 * API Type: Immutable API.
	 *
	 * @param v vector to interpolate with.
	 * @param t factor to interpolate; should be between in 0 to 1, inclusive.
	 * @returns the result of the linear interpolation.
	 */
	mix: (v: Vec2Source, t: number) => Vec2;
	/**
	 * Return a new vector with the same direction as this vector, but with a length of 1.
	 *
	 * API Type: Immutable API.
	 *
	 * @returns a new vector with the same direction as this vector, but with a length of 1.
	 */
	normalize: () => Vec2;
	/**
	 * Add two vectors together.
	 *
	 * API Type: Immutable API.
	 *
	 * @param v vector to add.
	 * @returns the result of adding v to this vector.
	 */
	plus: (v: Vec2Source) => Vec2;
	/**
	 * Multiply the vector by a scalar.
	 *
	 * API Type: Immutable API.
	 *
	 * @param s scalar to multiply by.
	 * @returns the result of multiplying this vector by s.
	 */
	scale: (s: number) => Vec2;
	/**
	 * Element-wise vector multiplication.
	 *
	 * API Type: Immutable API.
	 *
	 * @param v vector to multiply by.
	 * @returns the result of multiplying each element of this vector by each element of v.
	 */
	times: (v: Vec2Source) => Vec2;
	/**
	 * Element-wise vector division. Store the result in this Vec2 and return this Vec2 for
	 * chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @param v vector to divide by.
	 * @returns this vector for chaining.
	 */
	setDivide: (v: Vec2Source) => Vec2;
	/**
	 * Subtract a vector from this vector. Store the result in this Vec2 and return this Vec2
	 * for chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @param v vector to subtract.
	 * @returns this vector for chaining.
	 */
	setMinus: (v: Vec2Source) => Vec2;
	/**
	 * Compute a linear interpolation between this vector and another vector v with a factor t such
	 * that the result is thisVec * (1 - t) + v * t. The factor t should be between 0 and 1. Store the
	 * result in this Vec2 and return this Vec2 for chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @param v vector to interpolate with.
	 * @param t factor to interpolate; should be between in 0 to 1, inclusive.
	 * @returns this vector for chaining.
	 */
	setMix: (v: Vec2Source, t: number) => Vec2;
	/**
	 * Set the vector to be a version of itself with the same direction, but with length 1. Store the
	 * result in this Vec2 and return this Vec2 for chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @returns this vector for chaining.
	 */
	setNormalize: () => Vec2;
	/**
	 * Add two vectors together. Store the result in this Vec2 and return this Vec2 for chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @param v vector to add.
	 * @returns this vector for chaining.
	 */
	setPlus: (v: Vec2Source) => Vec2;
	/**
	 * Multiply the vector by a scalar. Store the result in this Vec2 and return this Vec2 for
	 * chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @param s scalar to multiply by.
	 * @returns this vector for chaining.
	 */
	setScale: (s: number) => Vec2;
	/**
	 * Element-wise vector multiplication. Store the result in this Vec2 and return this Vec2
	 * for chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @param v vector to multiply by.
	 * @returns this vector for chaining.
	 */
	setTimes: (v: Vec2Source) => Vec2;
	/**
	 * Set the Vec2's x component. Store the result in this Vec2 and return this for chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @param v value to set this vector's x component to.
	 * @returns this vector for chaining.
	 */
	setX(v: number): Vec2;
	/**
	 * Set the Vec2's y component. Store the result in this Vec2 and return this Vec2 for chaining.
	 *
	 * API Type: Mutable API.
	 *
	 * @param v value to set this vector's y component to.
	 * @returns this vector for chaining.
	 */
	setY(v: number): Vec2;
	/**
	 * Set the Vec2 to be all ones. Store the result in this Vec2 and return this Vec2 for chaining.
	 *
	 * API Type: Set API.
	 *
	 * @returns this vector for chaining.
	 */
	makeOne: () => Vec2;
	/**
	 * Set the Vec2 to have all components set to the scale value s. Store the result in this Vec2
	 * and return this Vec2 for chaining.
	 *
	 * API Type: Set API.
	 *
	 * @param s value to set all components to.
	 * @returns this vector for chaining.
	 */
	makeScale: (s: number) => Vec2;
	/**
	 * Set the Vec2 to be all zeros. Store the result in this Vec2 and return this Vec2 for
	 * chaining.
	 *
	 * API Type: Set API.
	 *
	 * @returns this vector for chaining.
	 */
	makeZero: () => Vec2;
	/**
	 * Set this Vec2 to have the same value as another Vec2. Store the result in this Vec2 and return
	 * this Vec2 for chaining.
	 *
	 * API Type: Set API.
	 *
	 * @param v vector to copy from.
	 * @returns this vector for chaining.
	 */
	setFrom: (v: Vec2Source) => Vec2;
	/**
	 * Set the Vec2's x and y components. Store the result in this Vec2 and return this for chaining.
	 *
	 * API Type: Set API.
	 *
	 * @param x value to set this vector's x component to.
	 * @param y value to set this vector's y component to.
	 * @returns this vector for chaining.
	 */
	setXy: (x: number, y: number) => Vec2;
}
interface Vec2Factory {
	/**
	 * Create a Vec2 from an object with x, y properties.
	 *
	 * API Type: Factory API.
	 *
	 * @param source to copy.
	 * @returns a new Vec2 with the same components as the source.
	 */
	from: (source: Vec2Source) => Vec2;
	/**
	 * Create a Vec2 with all elements set to one.
	 *
	 * API Type: Factory API.
	 *
	 * @returns a new Vec2 with all elements set to one.
	 */
	one: () => Vec2;
	/**
	 * Create a Vec2 with all elements set to the scale value s.
	 *
	 * API Type: Factory API.
	 *
	 * @param s value to set all components to.
	 * @returns a new Vec2 with all elements set to the scale value s.
	 */
	scale: (s: number) => Vec2;
	/**
	 * Create a Vec2 from x, y, z values.
	 *
	 * API Type: Factory API.
	 *
	 * @param x value to set the x component to.
	 * @param y value to set the y component to.
	 * @returns a new Vec2 with the x and y components set to the specified values.
	 */
	xy: (x: number, y: number) => Vec2;
	/**
	 * Create a Vec2 with all components set to zero.
	 *
	 * API Type: Factory API.
	 *
	 * @returns a new Vec2 with all components set to zero.
	 */
	zero: () => Vec2;
}
declare const vec2: Vec2Factory;
type ShadowLights = DirectionalLight | PointLight | SpotLight;
type NoShadowLights = AmbientLight | RectAreaLight;
type Lights = ShadowLights | NoShadowLights;
type CameraObject = OrthographicCamera | PerspectiveCamera;
type CameraManager = {
	getActiveEid: () => Eid;
	setActiveEid: (eid: Eid) => void;
	notifyCameraAdded: (eid: Eid) => void;
	notifyCameraRemoved: (eid: Eid) => void;
	attach: () => void;
	detach: () => void;
};
export interface ActiveCameraChangeEvent {
	camera: CameraObject;
}
export interface ActiveCameraEidChangeEvent {
	eid: Eid;
}
type XrCameraInfo = unknown;

type Overrides = {
	parent: Object3D | null;
	children: Object3D[];
};
type Object3D = Omit<OriginalObject3D, keyof Overrides> & Overrides;
interface InputManagerApi {
	setActiveMap: (name: string) => void;
	getActiveMap: () => string;
	getAction: (action: string) => number;
	readInputMap: (inputMap: DeepReadonly<InputMap>) => void;
}
interface InputListenerApi {
	getAxis: (gamepadIdx?: number) => DeepReadonly<number[]> | undefined;
	getGamepads: () => DeepReadonly<Gamepad[]>;
	getKey: (code: string) => boolean;
	getKeyDown: (code: string) => boolean;
	getKeyUp: (code: string) => boolean;
	getButton: (input: number, gamepadIdx?: number) => boolean;
	getButtonDown: (input: number, gamepadIdx?: number) => boolean;
	getButtonUp: (input: number, gamepadIdx?: number) => boolean;
	enablePointerLockRequest: () => void;
	disablePointerLockRequest: () => void;
	isPointerLockActive: () => boolean;
	exitPointerLock: () => void;
	getMouseButton: (value: number) => boolean;
	getMouseDown: (value: number) => boolean;
	getMouseUp: (value: number) => boolean;
	getMousePosition: () => DeepReadonly<[
		number,
		number
	]>;
	getMouseVelocity: () => DeepReadonly<[
		number,
		number
	]>;
	getMouseScroll: () => DeepReadonly<[
		number,
		number
	]>;
	getTouch: (identifier?: number) => boolean;
	getTouchDown: (identifier?: number) => boolean;
	getTouchUp: (identifier?: number) => boolean;
	getTouchIds: () => number[];
}
interface InputApi extends InputListenerApi, InputManagerApi {
	attach: () => void;
	detach: () => void;
}
interface WorldEffectCameraSchema {
	disableWorldTracking: boolean;
	enableLighting: boolean;
	enableWorldPoints: boolean;
	leftHandedAxes: boolean;
	mirroredDisplay: boolean;
	scale: string;
	direction: string;
	allowedDevices: string;
	enableVps: boolean;
}
interface FaceEffectCameraSchema {
	nearClip: number;
	farClip: number;
	direction: string;
	meshGeometryFace: boolean;
	meshGeometryEyes: boolean;
	meshGeometryIris: boolean;
	meshGeometryMouth: boolean;
	uvType: string;
	maxDetections: number;
	enableEars: boolean;
	mirroredDisplay: boolean;
	allowedDevices: string;
}
interface EcsRenderOverride {
	engage(): void;
	disengage(): void;
	render(dt: number): void;
}
type XrManager = {
	createWorldEffect: (config: Partial<WorldEffectCameraSchema>, eid: Eid) => number;
	startCameraPipeline: (handle: number) => void;
	stopCameraPipeline: (handle: number) => void;
	createFaceEffect: (config: Partial<FaceEffectCameraSchema>, eid: Eid) => number;
	startMediaRecorder: () => void;
	stopMediaRecorder: () => void;
	takeScreenshot: () => Promise<Blob>;
	drawPausedBackground: () => void;
	setEcsRenderOverride: (renderOverride: EcsRenderOverride) => void;
	attach: () => void;
	detach: () => void;
	tick: () => void;
	tock: () => void;
};
interface FrameInfo {
	elapsedTimeMs: number;
	maxRecordingMs: number;
	ctx: CanvasRenderingContext2D;
	canvas: HTMLCanvasElement;
}
interface ProgressInfo {
	progress: number;
	total: number;
}
type QueuedEvent<D = unknown> = {
	target: Eid;
	currentTarget: Eid;
	name: string;
	data: D;
};
type EventListener$1<D = unknown> = (event: QueuedEvent<D>) => void;
declare global {
	interface EcsEventTypes {
	}
}
type EcsEventTypes$1 = globalThis.EcsEventTypes;
type DataForEvent<EVENT> = EVENT extends keyof EcsEventTypes$1 ? EcsEventTypes$1[EVENT] : unknown;
interface Events {
	globalId: Eid;
	addListener: <T extends string>(target: Eid, name: T, listener: EventListener$1<DataForEvent<T>>) => void;
	removeListener: (target: Eid, name: string, listener: EventListener$1) => void;
	dispatch: (target: Eid, name: string, data?: unknown) => void;
}
interface PointerApi {
	attach: () => void;
	detach: () => void;
}
type RaycastStage = {
	scene: Scene;
	getCamera: () => CameraObject;
	includeWorldPosition: boolean;
};
type IntersectionResult = {
	eid?: Eid;
	point: Vec3;
	distance: number;
	threeData: Intersection;
};
type TransformManager = {
	getLocalPosition(eid: Eid, out?: Vec3): Vec3;
	getLocalTransform(eid: Eid, out?: Mat4): Mat4;
	getWorldPosition(eid: Eid, out?: Vec3): Vec3;
	getWorldQuaternion(eid: Eid, out?: Quat): Quat;
	getWorldTransform(eid: Eid, out?: Mat4): Mat4;
	setLocalPosition(eid: Eid, position: Vec3Source): void;
	setLocalTransform(eid: Eid, mat4: Mat4): void;
	setWorldPosition(eid: Eid, position: Vec3Source): void;
	setWorldQuaternion(eid: Eid, rotation: QuatSource): void;
	setWorldTransform(eid: Eid, mat4: Mat4): void;
	translateSelf(eid: Eid, translation: Partial<Vec3Source>): void;
	translateLocal(eid: Eid, translation: Partial<Vec3Source>): void;
	translateWorld(eid: Eid, translation: Partial<Vec3Source>): void;
	rotateSelf(eid: Eid, rotation: QuatSource): void;
	rotateLocal(eid: Eid, rotation: QuatSource): void;
	lookAt(eid: Eid, other: Eid): void;
	lookAtLocal(eid: Eid, position: Vec3Source): void;
	lookAtWorld(eid: Eid, position: Vec3Source): void;
};
export type SchemaOf<T extends RootAttribute<Schema>> = T extends RootAttribute<infer P> ? P : never;
export type RootAttribute<T extends Schema> = {
	set(world: World, eid: Eid, data?: Partial<ReadData<T>>): void;
	get(world: World, eid: Eid): ReadData<T>;
	has(world: World, eid: Eid): boolean;
	cursor(world: World, eid: Eid): WriteData<T>;
	mutate: (world: World, eid: Eid, fn: (cursor: WriteData<T>) => void | boolean) => void;
	acquire(world: World, eid: Eid): WriteData<T>;
	commit(world: World, eid: Eid, modified?: boolean): void;
	reset(world: World, eid: Eid): void;
	remove(world: World, eid: Eid): void;
	dirty(world: World, eid: Eid): void;
	forWorld: (world: World) => WorldAttribute<T>;
	schema: T | undefined;
	orderedSchema: OrderedSchema;
	defaults: Partial<ReadData<T>> | undefined;
};
export type WorldAttribute<T extends Schema> = {
	id: number;
	set(eid: Eid, data?: Partial<ReadData<T>>): void;
	get(eid: Eid): ReadData<T>;
	has(eid: Eid): boolean;
	cursor(eid: Eid): WriteData<T>;
	mutate(eid: Eid, fn: (cursor: WriteData<T>) => void | boolean): void;
	acquire(eid: Eid): WriteData<T>;
	commit(eid: Eid, modified?: boolean): void;
	reset(eid: Eid): void;
	remove(eid: Eid): void;
	dirty(eid: Eid): void;
};
type FunctionWithoutEid<Fn extends (eid: Eid, ...args: any[]) => any> = (Fn extends (eid: Eid, ...args: infer A) => infer R ? (...args: A) => R : never);
type EntityTransformManager = {
	[K in keyof TransformManager]: FunctionWithoutEid<TransformManager[K]>;
} & {
	lookAt: (other: Eid | Entity) => void;
};
export type Entity = EntityTransformManager & {
	eid: Eid;
	get: <S extends Schema>(component: RootAttribute<S>) => ReadData<S>;
	has: <S extends Schema>(component: RootAttribute<S>) => boolean;
	set: <S extends Schema>(component: RootAttribute<S>, data: Partial<ReadData<S>>) => void;
	remove: <S extends Schema>(component: RootAttribute<S>) => void;
	reset: <S extends Schema>(component: RootAttribute<S>) => void;
	hide(): void;
	show(): void;
	isHidden(): boolean;
	disable(): void;
	enable(): void;
	isDisabled(): boolean;
	delete(): void;
	isDeleted(): boolean;
	setParent(parent: Eid | Entity | undefined | null): void;
	getChildren(): Entity[];
	getParent(): Entity | null;
	addChild(child: Eid | Entity): void;
};
type EffectsManager = {
	setFog: (fog: DeepReadonly<Fog> | undefined) => void;
	getFog: () => DeepReadonly<Fog> | undefined;
	setSky: (sky: DeepReadonly<EffectsManagerSky> | undefined) => void;
	getSky: () => DeepReadonly<EffectsManagerSky> | undefined;
	attach: () => void;
	detach: () => void;
};
type EffectsManagerSky = Sky<string>;
type MatrixUpdateMode = "auto" | "manual";
interface ThreeState {
	renderer: WebGLRenderer;
	activeCamera: CameraObject;
	entityToObject: Map<Eid, Object3D>;
	scene: Scene;
	/**
	 * By default, 'manual' uses more efficient matrix update logic, but requires you to call
	 * `world.three.notifyChanged` after moving or reparenting raw three.js objects.
	 * If it's preferred have all matrices recalculated on each frame, set to 'auto'.
	 */
	setMatrixUpdateMode(mode: MatrixUpdateMode): void;
	/**
	 * When in manual matrix update mode, call notifyChanged after moving or reparenting
	 * raw three.js objects.
	 */
	notifyChanged: (object: Object3D) => void;
}
interface BaseWorld {
	time: Time;
	allEntities: Set<Eid>;
	eidToEntity: Map<Eid, Entity>;
	three: ThreeState;
	insertRaycastStage: (stage: RaycastStage, idx: number) => void;
	/** @deprecated */
	scene: Scene;
}
type TickMode = "partial" | "full" | "zero";
interface LateWorld {
	start: () => void;
	stop: () => void;
	tick: (dt?: number) => void;
	tock: () => void;
	getTickMode: () => TickMode;
	setTickMode: (tickMode: TickMode) => void;
	destroy: () => void;
	loadScene: (scene: DeepReadonly<SceneGraph>, callback?: (handle: SceneHandle) => void) => SceneHandle;
	createEntity: (prefabNameOrEid?: string | Eid) => Eid;
	deleteEntity: (eid: Eid) => void;
	getInstanceEntity: (instanceEid: Eid, prefabChildEid: Eid) => Eid;
	spawnIntoObject: (eid: Eid, object: DeepReadonly<BaseGraphObject>, graphIdToEid: Map<string, Eid>) => void;
	setScale: (eid: Eid, x: number, y: number, z: number) => void;
	setPosition: (eid: Eid, x: number, y: number, z: number) => void;
	setQuaternion: (eid: Eid, x: number, y: number, z: number, w: number) => void;
	setTransform: (eid: Eid, transform: Mat4) => void;
	getWorldTransform: (eid: Eid, transform: Mat4) => void;
	normalizeQuaternion: (eid: Eid) => void;
	setParent: (eid: Eid, parent: Eid) => void;
	getParent: (eid: Eid) => Eid;
	getChildren: (eid: Eid) => Generator<Eid>;
	raycast: (origin: Vec3Source, direction: Vec3Source, near?: number, far?: number) => IntersectionResult[];
	raycastFrom: (eid: Eid, near?: number, far?: number) => IntersectionResult[];
	audio: AudioControls;
	camera: CameraManager;
	pointer: PointerApi;
	events: Events;
	getEntity: (eid: Eid) => Entity;
	transform: TransformManager;
	input: InputApi;
	xr: XrManager;
	setSceneHook: (hook: SpacesHandle & PrefabsHandle) => void;
	spaces: SpacesHandle;
	effects: EffectsManager;
}
export interface World extends BaseWorld, LateWorld {
}
export declare const createWorld: (scene: Scene, renderer: WebGLRenderer, camera: CameraObject) => World;
type TransitionCallback<CallbackArgument = void> = CallbackArgument extends void ? () => void : (arg: CallbackArgument) => void;
export interface State<CallbackArgument = void> {
	triggers: Record<string, Trigger[]>;
	onEnter?: TransitionCallback<CallbackArgument>;
	onTick?: TransitionCallback<CallbackArgument>;
	onExit?: () => void;
	listeners?: ListenerParams[];
}
interface IStateDefiner<CallbackArgument = void> {
	name: string;
	initial: () => this;
	onEnter: (cb: State<CallbackArgument>["onEnter"]) => this;
	onTick: (cb: State<CallbackArgument>["onTick"]) => this;
	onExit: (cb: State<CallbackArgument>["onExit"]) => this;
	onEvent: <T extends string>(event: T, nextState: StateId, args?: Omit<EventTrigger<T>, "type" | "event">) => this;
	wait: (timeout: number, nextState: StateId) => this;
	onTrigger: (trigger: TriggerHandle, nextState: StateId) => this;
	listen: <T extends string>(target: EidGetter, name: T, listener: EventListener$1<DataForEvent<T>>) => this;
}
type StateId = string | {
	name: string;
};
export interface StateGroup<CallbackArgument = void> {
	substates?: StateId[];
	triggers: Record<string, Trigger[]>;
	onEnter?: TransitionCallback<CallbackArgument>;
	onTick?: TransitionCallback<CallbackArgument>;
	onExit?: () => void;
	listeners?: ListenerParams[];
}
interface IStateGroupDefiner<CallbackArgument = void> {
	onEnter: (cb: StateGroup<CallbackArgument>["onEnter"]) => this;
	onTick: (cb: StateGroup<CallbackArgument>["onTick"]) => this;
	onExit: (cb: StateGroup<CallbackArgument>["onExit"]) => this;
	onEvent: (event: string, nextState: StateId, args?: Omit<EventTrigger, "type" | "event">) => this;
	wait: (timeout: number, nextState: StateId) => this;
	onTrigger: (trigger: TriggerHandle, nextState: StateId) => this;
	listen: (target: EidGetter, name: string, listener: EventListener$1) => this;
}
export type MachineId = number;
export interface StateMachineDefinition<CallbackArgument = void> {
	initialState: string;
	states: Record<string, State<CallbackArgument>>;
	groups?: StateGroup<CallbackArgument>[];
	prepareCallback?: CallbackArgument extends void ? never : () => CallbackArgument;
}
export interface BaseMachineDefProps {
	world: World;
	eid: Eid;
	entity: Entity;
}
export type StateMachineDefiner = (props: BaseMachineDefProps) => void;
type ComponentCallbackArgs<S extends Schema, D extends Schema> = {
	schema: WriteData<S>;
	data: WriteData<D>;
};
interface ComponentStateMachineDefProps<S extends Schema, D extends Schema> extends BaseMachineDefProps {
	schemaAttribute: WorldAttribute<S>;
	dataAttribute: WorldAttribute<D>;
	defineState: (name: string) => IStateDefiner<ComponentCallbackArgs<S, D>>;
	defineStateGroup: (substates?: Array<StateId | IStateGroupDefiner<unknown>>) => IStateGroupDefiner<ComponentCallbackArgs<S, D>>;
}
type ComponentStateMachineDefiner<S extends Schema, D extends Schema> = (props: ComponentStateMachineDefProps<S, D>) => void;
type ComponentStateMachineDefinition<S extends Schema, D extends Schema> = Omit<StateMachineDefinition<ComponentCallbackArgs<S, D>>, "prepareCallback">;
type EventTrigger<T extends string = string> = {
	type: "event";
	event: T;
	target?: Eid;
	where?: (event: QueuedEvent<DataForEvent<T>>) => boolean;
	/** @deprecated */
	beforeTransition?: (event: QueuedEvent<DataForEvent<T>>) => boolean;
};
type TimeoutTrigger = {
	type: "timeout";
	timeout: number;
};
type Callback$1 = () => void;
type TriggerHandle = {
	trigger: () => void;
	listen: (cb: Callback$1) => void;
	unlisten: (cb: Callback$1) => void;
};
type CustomTrigger = {
	type: "custom";
	handle: TriggerHandle;
};
type Trigger = EventTrigger<any> | TimeoutTrigger | CustomTrigger;
type EidGetter = Eid | (() => Eid);
type ListenerParams = {
	target: EidGetter;
	name: string;
	listener: EventListener$1<any>;
};
type ExtendedSchemaValue<T extends Type> = T | [
	T
] | [
	T,
	TypeToValue[T]
];
type ExtendedSchema<S extends Schema> = {
	[key in keyof S]: ExtendedSchemaValue<S[key]>;
};
type BaseSchema<S extends ExtendedSchema<Schema>> = {
	[K in keyof S]: S[K] extends ExtendedSchemaValue<infer T> ? T : never;
};
type WorldBehavior = (w: World) => void;
declare const behaviors: WorldBehavior[];
export declare const registerBehavior: (callback: WorldBehavior) => void;
export declare const unregisterBehavior: (callback: WorldBehavior) => void;
export declare const getBehaviors: () => DeepReadonly<typeof behaviors>;
type ComponentCursor<S extends Schema, D extends Schema> = {
	eid: Eid;
	schema: WriteData<S>;
	data: WriteData<D>;
	schemaAttribute: WorldAttribute<S>;
	dataAttribute: WorldAttribute<D>;
};
type RemovedComponentCursor<S extends Schema, D extends Schema> = Omit<ComponentCursor<S, D>, "schema" | "data">;
type ComponentRegistration<ES extends ExtendedSchema<Schema>, ED extends ExtendedSchema<Schema>> = {
	name: string;
	/**
	 * Add data that can be configured on the component.
	 */
	schema?: ES;
	/**
	 * Add defaults for the schema fields.
	 */
	schemaDefaults?: Partial<ReadData<BaseSchema<ES>>>;
	/**
	 * Add data that cannot be configured outside of the component.
	 */
	data?: ED;
	/**
	 * Runs when the component is added to an entity.
	 */
	add?: (w: World, cursor: ComponentCursor<BaseSchema<ES>, BaseSchema<ED>>) => void;
	/**
	 * Runs every frame for each entity.
	 */
	tick?: (w: World, cursor: ComponentCursor<BaseSchema<ES>, BaseSchema<ED>>) => void;
	/**
	 * Runs when the component is removed from an entity.
	 */
	remove?: (w: World, cursor: RemovedComponentCursor<BaseSchema<ES>, BaseSchema<ED>>) => void;
	/**
	 * Define stateful behaviors such as event handling and transitions.
	 */
	stateMachine?: ComponentStateMachineDefinition<BaseSchema<ES>, BaseSchema<ED>> | ComponentStateMachineDefiner<BaseSchema<ES>, BaseSchema<ED>>;
};
export declare const registerComponent: <ES extends ExtendedSchema<Schema>, ED extends ExtendedSchema<Schema>>({ name, schema, schemaDefaults, stateMachine: machineDef, data, tick, add, remove, }: ComponentRegistration<ES, ED>) => RootAttribute<BaseSchema<ES>>;
export declare const getAttribute: (name: string) => RootAttribute<{}>;
export declare const listAttributes: () => string[];
declare const Audio$1: RootAttribute<{
	url: "string";
	volume: "f32";
	loop: "boolean";
	paused: "boolean";
	pitch: "f32";
	positional: "boolean";
	refDistance: "f32";
	rolloffFactor: "f32";
	distanceModel: "string";
	maxDistance: "f32";
}>;
declare const Camera$2: RootAttribute<{
	type: "string";
	fov: "f32";
	zoom: "f32";
	left: "i32";
	right: "i32";
	top: "i32";
	bottom: "i32";
	xrCameraType: "string";
	phone: "string";
	desktop: "string";
	headset: "string";
	nearClip: "f32";
	farClip: "f32";
	leftHandedAxes: "boolean";
	uvType: "string";
	direction: "string";
	disableWorldTracking: "boolean";
	enableLighting: "boolean";
	enableWorldPoints: "boolean";
	scale: "string";
	enableVps: "boolean";
	mirroredDisplay: "boolean";
	meshGeometryFace: "boolean";
	meshGeometryEyes: "boolean";
	meshGeometryIris: "boolean";
	meshGeometryMouth: "boolean";
	enableEars: "boolean";
	maxDetections: "i32";
}>;
declare const Face$1: RootAttribute<{
	id: "i32";
	addAttachmentState: "boolean";
}>;
declare const ImageTarget$1: RootAttribute<{
	name: "string";
}>;
declare const SphereGeometry$1: RootAttribute<{
	radius: "f32";
}>;
declare const BoxGeometry$1: RootAttribute<{
	width: "f32";
	height: "f32";
	depth: "f32";
}>;
declare const PlaneGeometry$1: RootAttribute<{
	width: "f32";
	height: "f32";
}>;
declare const CapsuleGeometry$1: RootAttribute<{
	radius: "f32";
	height: "f32";
}>;
declare const ConeGeometry$1: RootAttribute<{
	radius: "f32";
	height: "f32";
}>;
declare const CylinderGeometry$1: RootAttribute<{
	radius: "f32";
	height: "f32";
}>;
declare const TetrahedronGeometry$1: RootAttribute<{
	radius: "f32";
}>;
declare const PolyhedronGeometry$1: RootAttribute<{
	faces: "ui8";
	radius: "f32";
}>;
declare const CircleGeometry$1: RootAttribute<{
	radius: "f32";
}>;
declare const RingGeometry$1: RootAttribute<{
	innerRadius: "f32";
	outerRadius: "f32";
}>;
declare const TorusGeometry$1: RootAttribute<{
	radius: "f32";
	tubeRadius: "f32";
}>;
declare const FaceGeometry$1: RootAttribute<{}>;
declare const GltfModel$1: RootAttribute<{
	url: "string";
	animationClip: "string";
	loop: "boolean";
	paused: "boolean";
	time: "f32";
	timeScale: "f32";
	collider: "boolean";
	reverse: "boolean";
	repetitions: "ui32";
	crossFadeDuration: "f32";
}>;
export declare const Hidden: RootAttribute<Schema>;
declare const Light$1: RootAttribute<{
	type: "string";
	r: "ui8";
	g: "ui8";
	b: "ui8";
	intensity: "f32";
	castShadow: "boolean";
	targetX: "f32";
	targetY: "f32";
	targetZ: "f32";
	shadowNormalBias: "f32";
	shadowBias: "f32";
	shadowAutoUpdate: "boolean";
	shadowBlurSamples: "ui32";
	shadowRadius: "f32";
	shadowMapSizeHeight: "i32";
	shadowMapSizeWidth: "i32";
	shadowCameraNear: "f32";
	shadowCameraFar: "f32";
	shadowCameraLeft: "f32";
	shadowCameraRight: "f32";
	shadowCameraTop: "f32";
	shadowCameraBottom: "f32";
	distance: "f32";
	decay: "f32";
	followCamera: "boolean";
	angle: "f32";
	penumbra: "f32";
	colorMap: "string";
	width: "f32";
	height: "f32";
}>;
declare const Material$2: RootAttribute<{
	r: "ui8";
	g: "ui8";
	b: "ui8";
	textureSrc: "string";
	roughness: "f32";
	metalness: "f32";
	opacity: "f32";
	roughnessMap: "string";
	metalnessMap: "string";
	side: "string";
	normalScale: "f32";
	emissiveIntensity: "f32";
	emissiveR: "ui8";
	emissiveG: "ui8";
	emissiveB: "ui8";
	opacityMap: "string";
	normalMap: "string";
	emissiveMap: "string";
	blending: "string";
	repeatX: "f32";
	repeatY: "f32";
	offsetX: "f32";
	offsetY: "f32";
	wrap: "string";
	depthTest: "boolean";
	depthWrite: "boolean";
	wireframe: "boolean";
	forceTransparent: "boolean";
	textureFiltering: "string";
	mipmaps: "boolean";
}>;
declare const UnlitMaterial$1: RootAttribute<{
	r: "ui8";
	g: "ui8";
	b: "ui8";
	textureSrc: "string";
	opacity: "f32";
	side: "string";
	opacityMap: "string";
	blending: "string";
	repeatX: "f32";
	repeatY: "f32";
	offsetX: "f32";
	offsetY: "f32";
	wrap: "string";
	depthTest: "boolean";
	depthWrite: "boolean";
	wireframe: "boolean";
	forceTransparent: "boolean";
	textureFiltering: "string";
	mipmaps: "boolean";
}>;
declare const ShadowMaterial$1: RootAttribute<{
	r: "ui8";
	g: "ui8";
	b: "ui8";
	opacity: "f32";
	side: "string";
	depthTest: "boolean";
	depthWrite: "boolean";
}>;
declare const HiderMaterial$1: RootAttribute<{}>;
declare const VideoMaterial$1: RootAttribute<{
	r: "ui8";
	g: "ui8";
	b: "ui8";
	textureSrc: "string";
	opacity: "f32";
}>;
export declare const Persistent: RootAttribute<Schema>;
declare const Shadow$1: RootAttribute<{
	castShadow: "boolean";
	receiveShadow: "boolean";
}>;
declare const Splat$1: RootAttribute<{
	url: "string";
	skybox: "boolean";
}>;
export declare const Position: RootAttribute<{
	readonly x: "f32";
	readonly y: "f32";
	readonly z: "f32";
}>;
export declare const Scale: RootAttribute<{
	readonly x: "f32";
	readonly y: "f32";
	readonly z: "f32";
}>;
declare const Quaternion$1: RootAttribute<{
	readonly x: "f32";
	readonly y: "f32";
	readonly z: "f32";
	readonly w: "f32";
}>;
export declare const ThreeObject: RootAttribute<{
	order: "f32";
}>;
export declare const Ui: RootAttribute<{
	type: "string";
	font: "string";
	fontSize: "f32";
	position: "string";
	opacity: "f32";
	backgroundOpacity: "f32";
	backgroundSize: "string";
	nineSliceBorderTop: "string";
	nineSliceBorderBottom: "string";
	nineSliceBorderLeft: "string";
	nineSliceBorderRight: "string";
	nineSliceScaleFactor: "f32";
	background: "string";
	color: "string";
	text: "string";
	image: "string";
	fixedSize: "boolean";
	width: "string";
	height: "string";
	top: "string";
	left: "string";
	bottom: "string";
	right: "string";
	borderColor: "string";
	borderRadius: "f32";
	borderRadiusTopLeft: "string";
	borderRadiusTopRight: "string";
	borderRadiusBottomLeft: "string";
	borderRadiusBottomRight: "string";
	ignoreRaycast: "boolean";
	alignContent: "string";
	alignItems: "string";
	alignSelf: "string";
	borderWidth: "f32";
	columnGap: "string";
	direction: "string";
	display: "string";
	flex: "f32";
	flexBasis: "string";
	flexDirection: "string";
	flexGrow: "f32";
	flexShrink: "f32";
	flexWrap: "string";
	gap: "string";
	justifyContent: "string";
	margin: "string";
	marginBottom: "string";
	marginLeft: "string";
	marginRight: "string";
	marginTop: "string";
	maxHeight: "string";
	maxWidth: "string";
	minHeight: "string";
	minWidth: "string";
	overflow: "string";
	padding: "string";
	paddingBottom: "string";
	paddingLeft: "string";
	paddingRight: "string";
	paddingTop: "string";
	rowGap: "string";
	textAlign: "string";
	verticalTextAlign: "string";
	stackingOrder: "f32";
}>;
export declare const VideoControls: RootAttribute<{
	loop: "boolean";
	paused: "boolean";
	volume: "f32";
	positional: "boolean";
	speed: "f32";
	refDistance: "f32";
	rolloffFactor: "f32";
	distanceModel: "string";
	maxDistance: "f32";
}>;
/**
 * Function to define a new state
 * @param name the name of the state
 * @returns a new state
 */
export declare const defineState: <CallbackArgument = void>(name: string) => IStateDefiner<CallbackArgument>;
/**
 * Function to define a new group
 * @param substates the substates of the group (leaving blank will capture all states)
 * @returns a new group
 */
export declare const defineStateGroup: <CallbackArgument = void>(substates?: Array<StateId | IStateGroupDefiner<unknown>>) => IStateGroupDefiner<CallbackArgument>;
/**
 * define a custom trigger that can be called to cause a transition
 * @returns a new custom trigger definition
 */
export declare const defineTrigger: () => TriggerHandle;
/**
 * Create a state machine
 * @param world the world to create the state machine in
 * @param eid the entity that owns the state machine
 * @param definition the state machine definition. This can be either an object or a function that
 *                   generate the definition object
 * @returns the id of the created state machine
 */
export declare const createStateMachine: <CallbackArgument = void>(world: World, eid: Eid, definition: StateMachineDefinition<CallbackArgument> | StateMachineDefiner) => MachineId;
export declare const deleteStateMachine: (world: World, machineId: MachineId) => void;
export declare const tickStateMachine: (world: World, machineId: MachineId) => void;
export interface GamepadConnectedEvent {
	gamepad: Gamepad;
}
export interface GamepadDisconnectedEvent {
	gamepad: Gamepad;
}
declare const GAMEPAD_CONNECTED: "input-gamepad-connected";
declare const GAMEPAD_DISCONNECTED: "input-gamepad-disconnected";
declare const SCREEN_TOUCH_START: "screen-touch-start";
declare const SCREEN_TOUCH_MOVE: "screen-touch-move";
declare const SCREEN_TOUCH_END: "screen-touch-end";
declare const GESTURE_START: "gesture-start";
declare const GESTURE_MOVE: "gesture-move";
declare const GESTURE_END: "gesture-end";
type ScreenPosition = {
	x: number;
	y: number;
};
type PointerId = PointerEvent["pointerId"];
export interface ScreenTouchStartEvent {
	pointerId: PointerId;
	position: ScreenPosition;
	target: Eid | undefined;
	worldPosition: Vec3 | undefined;
}
export interface ScreenTouchMoveEvent {
	pointerId: PointerId;
	position: ScreenPosition;
	start: ScreenPosition;
	change: ScreenPosition;
	target: Eid | undefined;
}
export interface ScreenTouchEndEvent {
	pointerId: PointerId;
	position: ScreenPosition;
	start: ScreenPosition;
	target: Eid | undefined;
	endTarget: Eid | undefined;
	worldPosition: Vec3 | undefined;
}
export interface GestureStartEvent {
	startPosition: ScreenPosition;
	position: ScreenPosition;
	startSpread: number;
	spread: number;
	touchCount: number;
}
export interface GestureMoveEvent {
	startPosition: ScreenPosition;
	position: ScreenPosition;
	positionChange: ScreenPosition;
	startSpread: number;
	spread: number;
	touchCount: number;
	spreadChange: number;
}
export interface GestureEndEvent {
	startPosition: ScreenPosition;
	position: ScreenPosition;
	startSpread: number;
	spread: number;
	touchCount: number;
	target: Eid | undefined;
	nextTouchCount: number | undefined;
}
export type UiClickEvent = {
	x: number;
	y: number;
};
export type UiHoverEvent = {
	x: number;
	y: number;
	targets: Eid[];
};
declare const UI_CLICK: "click";
declare const UI_PRESSED: "ui-pressed";
declare const UI_RELEASED: "ui-released";
declare const UI_HOVER_START: "ui-hover-start";
declare const UI_HOVER_END: "ui-hover-end";
export declare const input: {
	SCREEN_TOUCH_START: "screen-touch-start";
	SCREEN_TOUCH_MOVE: "screen-touch-move";
	SCREEN_TOUCH_END: "screen-touch-end";
	GESTURE_START: "gesture-start";
	GESTURE_MOVE: "gesture-move";
	GESTURE_END: "gesture-end";
	GAMEPAD_CONNECTED: "input-gamepad-connected";
	GAMEPAD_DISCONNECTED: "input-gamepad-disconnected";
	UI_CLICK: "click";
	UI_PRESSED: "ui-pressed";
	UI_RELEASED: "ui-released";
	UI_HOVER_START: "ui-hover-start";
	UI_HOVER_END: "ui-hover-end";
};
export declare const eid = "eid";
export declare const f32 = "f32";
export declare const f64 = "f64";
export declare const i32 = "i32";
export declare const ui8 = "ui8";
export declare const ui32 = "ui32";
export declare const string = "string";
export declare const boolean = "boolean";
type Attributes = RootAttribute<{}>[];
type TableMatch<T extends Attributes> = {
	eids: Generator<Eid>;
	ptrs: {
		[K in keyof T]: number;
	};
	count: number;
};
type SystemQuery<T extends Attributes> = (world: World) => Generator<TableMatch<T>>;
type WriteDataForTerms<T extends Attributes> = {
	[K in keyof T]: WriteData<SchemaOf<T[K]>>;
};
type SystemCallback<T extends Attributes> = ((world: World, eid: Eid, cursors: WriteDataForTerms<T>) => void);
export declare const defineSystemQuery: <T extends Attributes>(terms: T) => SystemQuery<T>;
export declare const defineSystem: <T extends Attributes>(terms: T, callback: SystemCallback<T>) => (world: World) => void;
type Query = (world: World) => Eid[];
type LifecycleQueries = {
	init: (world: World) => void;
	enter: Query;
	changed: Query;
	exit: Query;
};
interface RootQuery extends Query {
	terms: RootAttribute<any>[];
}
export declare const defineQuery: (terms: RootAttribute<any>[]) => RootQuery;
export declare const enterQuery: (t: RootQuery) => Query;
export declare const exitQuery: (t: RootQuery) => Query;
export declare const changedQuery: (t: RootQuery) => Query;
export declare const lifecycleQueries: (t: RootQuery) => LifecycleQueries;
export declare const XR_FACE_FOUND: "facecontroller.facefound";
export declare const XR_FACE_UPDATED: "facecontroller.faceupdated";
export declare const XR_FACE_LOST: "facecontroller.facelost";
export declare const CameraEvents: {
	ACTIVE_CAMERA_CHANGE: "active-camera-change";
	ACTIVE_CAMERA_EID_CHANGE: "active-camera-entity-change";
	XR_CAMERA_EDIT: "xr-camera-edit";
	XR_CAMERA_STOP: "xr.stop";
	CAMERA_TRANSFORM_UPDATE: "cameraupdate";
};
export declare const events: {
	readonly ACTIVE_SPACE_CHANGE: "active-space-change";
	readonly AUDIO_CAN_PLAY_THROUGH: "audio-can-play-through";
	readonly AUDIO_END: "audio-end";
	readonly VIDEO_CAN_PLAY_THROUGH: "video-can-play-through";
	readonly VIDEO_END: "video-end";
	readonly GLTF_MODEL_LOADED: "gltf-model-loaded";
	readonly GLTF_ANIMATION_FINISHED: "gltf-animation-finished";
	readonly GLTF_ANIMATION_LOOP: "gltf-animation-loop";
	readonly SPLAT_MODEL_LOADED: "splat-model-loaded";
	readonly POSITION_ANIMATION_COMPLETE: "position-animation-complete";
	readonly SCALE_ANIMATION_COMPLETE: "scale-animation-complete";
	readonly ROTATE_ANIMATION_COMPLETE: "rotate-animation-complete";
	readonly CUSTOM_VEC3_ANIMATION_COMPLETE: "vector3-animation-complete";
	readonly CUSTOM_PROPERTY_ANIMATION_COMPLETE: "animation-complete";
	readonly LOCATION_SPAWNED: "locationSpawned";
	readonly RECORDER_VIDEO_STARTED: "recorder-video-started";
	readonly RECORDER_VIDEO_STOPPED: "recorder-video-stopped";
	readonly RECORDER_VIDEO_ERROR: "recorder-video-error";
	readonly RECORDER_VIDEO_READY: "recorder-video-ready";
	readonly RECORDER_FINALIZE_PROGRESS: "recorder-finalize-progress";
	readonly RECORDER_PREVIEW_READY: "recorder-preview-ready";
	readonly RECORDER_PROCESS_FRAME: "recorder-process-frame";
	readonly RECORDER_SCREENSHOT_READY: "recorder-screenshot-ready";
	readonly REALITY_CAMERA_CONFIGURED: "reality.cameraconfigured";
	readonly REALITY_TRACKING_STATUS: "reality.trackingstatus";
	readonly REALITY_LOCATION_SCANNING: "reality.locationscanning";
	readonly REALITY_LOCATION_FOUND: "reality.locationfound";
	readonly REALITY_LOCATION_UPDATED: "reality.locationupdated";
	readonly REALITY_LOCATION_LOST: "reality.locationlost";
	readonly REALITY_MESH_FOUND: "reality.meshfound";
	readonly REALITY_MESH_LOST: "reality.meshlost";
	readonly REALITY_IMAGE_LOADING: "reality.imageloading";
	readonly REALITY_IMAGE_SCANNING: "reality.imagescanning";
	readonly REALITY_IMAGE_FOUND: "reality.imagefound";
	readonly REALITY_IMAGE_UPDATED: "reality.imageupdated";
	readonly REALITY_IMAGE_LOST: "reality.imagelost";
	readonly REALITY_READY: "realityready";
	readonly FACE_CAMERA_CONFIGURED: "facecontroller.cameraconfigured";
	readonly FACE_LOADING: "facecontroller.faceloading";
	readonly FACE_SCANNING: "facecontroller.facescanning";
	readonly FACE_FOUND: "facecontroller.facefound";
	readonly FACE_UPDATED: "facecontroller.faceupdated";
	readonly FACE_LOST: "facecontroller.facelost";
	readonly FACE_MOUTH_OPENED: "facecontroller.mouthopened";
	readonly FACE_MOUTH_CLOSED: "facecontroller.mouthclosed";
	readonly FACE_LEFT_EYE_OPENED: "facecontroller.lefteyeopened";
	readonly FACE_LEFT_EYE_CLOSED: "facecontroller.lefteyeclosed";
	readonly FACE_RIGHT_EYE_OPENED: "facecontroller.righteyeopened";
	readonly FACE_RIGHT_EYE_CLOSED: "facecontroller.righteyeclosed";
	readonly FACE_LEFT_EYEBROW_RAISED: "facecontroller.lefteyebrowraised";
	readonly FACE_LEFT_EYEBROW_LOWERED: "facecontroller.lefteyebrowlowered";
	readonly FACE_RIGHT_EYEBROW_RAISED: "facecontroller.righteyebrowraised";
	readonly FACE_RIGHT_EYEBROW_LOWERED: "facecontroller.righteyebrowlowered";
	readonly FACE_RIGHT_EYE_WINKED: "facecontroller.righteyewinked";
	readonly FACE_LEFT_EYE_WINKED: "facecontroller.lefteyewinked";
	readonly FACE_BLINKED: "facecontroller.blinked";
	readonly FACE_INTERPUPILLARY_DISTANCE: "facecontroller.interpupillarydistance";
	readonly FACE_EAR_POINT_FOUND: "facecontroller.earpointfound";
	readonly FACE_EAR_POINT_LOST: "facecontroller.earpointlost";
	readonly HAND_CAMERA_CONFIGURED: "handcontroller.cameraconfigured";
	readonly LAYERS_CAMERA_CONFIGURED: "layerscontroller.cameraconfigured";
};
declare const COLLISION_START_EVENT: "physics-collision-start";
declare const COLLISION_END_EVENT: "physics-collision-end";
declare const UPDATE_EVENT: "physics-update";
export type LocationSpawnedEvent = {
	id: string;
	imageUrl: string;
	title: string;
	lat: number;
	lng: number;
	mapPoint: Eid;
};
declare global {
	interface EcsEventTypes {
		[events.RECORDER_VIDEO_ERROR]: Error;
		[events.RECORDER_VIDEO_READY]: {
			videoBlob: Blob;
		};
		[events.RECORDER_SCREENSHOT_READY]: Blob;
		[events.RECORDER_FINALIZE_PROGRESS]: ProgressInfo;
		[events.RECORDER_PREVIEW_READY]: {
			videoBlob: Blob;
		};
		[events.RECORDER_PROCESS_FRAME]: FrameInfo;
		[UI_CLICK]: {
			x: number;
			y: number;
		};
		[UI_PRESSED]: {
			x: number;
			y: number;
		};
		[UI_RELEASED]: {
			x: number;
			y: number;
		};
		[UI_HOVER_START]: UiHoverEvent;
		[UI_HOVER_END]: UiHoverEvent;
		[SCREEN_TOUCH_START]: ScreenTouchStartEvent;
		[SCREEN_TOUCH_MOVE]: ScreenTouchMoveEvent;
		[SCREEN_TOUCH_END]: ScreenTouchEndEvent;
		[GESTURE_START]: GestureStartEvent;
		[GESTURE_MOVE]: GestureMoveEvent;
		[GESTURE_END]: GestureEndEvent;
		[GAMEPAD_CONNECTED]: GamepadConnectedEvent;
		[GAMEPAD_DISCONNECTED]: GamepadDisconnectedEvent;
		[CameraEvents.ACTIVE_CAMERA_CHANGE]: {
			camera: CameraObject;
		};
		[CameraEvents.ACTIVE_CAMERA_EID_CHANGE]: {
			eid: Eid;
		};
		[CameraEvents.XR_CAMERA_STOP]: {};
		[CameraEvents.XR_CAMERA_EDIT]: {
			camera: CameraObject;
		};
		[events.SPLAT_MODEL_LOADED]: {
			model: Object3D;
		};
		[events.GLTF_MODEL_LOADED]: {
			model: Group;
		};
		[events.GLTF_ANIMATION_FINISHED]: {
			name: string;
		};
		[events.GLTF_ANIMATION_LOOP]: {
			name: string;
		};
		[events.AUDIO_END]: undefined;
		[events.POSITION_ANIMATION_COMPLETE]: undefined;
		[events.SCALE_ANIMATION_COMPLETE]: undefined;
		[events.ROTATE_ANIMATION_COMPLETE]: undefined;
		[events.CUSTOM_VEC3_ANIMATION_COMPLETE]: undefined;
		[events.CUSTOM_PROPERTY_ANIMATION_COMPLETE]: undefined;
		"audio-error": {
			error: Error;
		};
		[events.VIDEO_CAN_PLAY_THROUGH]: {
			src: string;
		};
		[events.VIDEO_END]: {
			src: string;
		};
		"video-error": {
			error: Error;
		};
		[COLLISION_START_EVENT]: {
			other: Eid;
		};
		[COLLISION_END_EVENT]: {
			other: Eid;
		};
		[UPDATE_EVENT]: {};
		[events.LOCATION_SPAWNED]: LocationSpawnedEvent;
	}
}
declare const colliderSchema: {
	readonly width: "f32";
	readonly height: "f32";
	readonly depth: "f32";
	readonly radius: "f32";
	readonly mass: "f32";
	readonly linearDamping: "f32";
	readonly angularDamping: "f32";
	readonly friction: "f32";
	readonly restitution: "f32";
	readonly gravityFactor: "f32";
	readonly offsetX: "f32";
	readonly offsetY: "f32";
	readonly offsetZ: "f32";
	readonly offsetQuaternionX: "f32";
	readonly offsetQuaternionY: "f32";
	readonly offsetQuaternionZ: "f32";
	readonly offsetQuaternionW: "f32";
	readonly shape: "ui32";
	readonly type: "ui8";
	readonly eventOnly: "boolean";
	readonly lockXPosition: "boolean";
	readonly lockYPosition: "boolean";
	readonly lockZPosition: "boolean";
	readonly lockXAxis: "boolean";
	readonly lockYAxis: "boolean";
	readonly lockZAxis: "boolean";
	readonly highPrecision: "boolean";
	readonly simplificationMode: "string";
};
declare const ColliderType$1: {
	readonly Static: 0;
	readonly Dynamic: 1;
	readonly Kinematic: 2;
};
export declare const ColliderShape: {
	readonly Box: 0;
	readonly Sphere: 1;
	readonly Plane: 2;
	readonly Capsule: 3;
	readonly Cone: 4;
	readonly Cylinder: 5;
	readonly Circle: 6;
};
type ColliderSchema = typeof colliderSchema;
declare const Collider$1: RootAttribute<ColliderSchema>;
export declare const physics: {
	enable: (world: World) => void;
	disable: (world: World) => void;
	setWorldGravity: (world: World, gravity: number) => void;
	getWorldGravity: (world: World) => number;
	applyForce: (world: World, eid: Eid, forceX: number, forceY: number, forceZ: number) => void;
	applyImpulse: (world: World, eid: Eid, impulseX: number, impulseY: number, impulseZ: number) => void;
	applyTorque: (world: World, eid: Eid, torqueX: number, torqueY: number, torqueZ: number) => void;
	setLinearVelocity: (world: World, eid: Eid, velocityX: number, velocityY: number, velocityZ: number) => void;
	getLinearVelocity: (world: World, eid: Eid) => {
		x: number;
		y: number;
		z: number;
	};
	setAngularVelocity: (world: World, eid: Eid, velocityX: number, velocityY: number, velocityZ: number) => void;
	getAngularVelocity: (world: World, eid: Eid) => {
		x: number;
		y: number;
		z: number;
	};
	registerConvexShape: (world: World, vertices: Float32Array) => number;
	unregisterConvexShape: (world: World, id: number) => void;
	COLLISION_START_EVENT: "physics-collision-start";
	COLLISION_END_EVENT: "physics-collision-end";
	UPDATE_EVENT: "physics-update";
	ColliderShape: {
		readonly Box: 0;
		readonly Sphere: 1;
		readonly Plane: 2;
		readonly Capsule: 3;
		readonly Cone: 4;
		readonly Cylinder: 5;
		readonly Circle: 6;
	};
	ColliderType: {
		readonly Static: 0;
		readonly Dynamic: 1;
		readonly Kinematic: 2;
	};
};
type AssetManifestMappings = {
	[filePath: string]: string;
} & {
	assets?: never;
};
type StoredAssetManifest = {
	assets: AssetManifestMappings;
};
type AssetManifest = StoredAssetManifest | AssetManifestMappings;
type Asset$1 = {
	data: Blob;
	remoteUrl?: string;
	localUrl: string;
};
type AssetManager = {
	load: (request: AssetRequest) => Promise<Asset$1>;
	clear: (request: AssetRequest) => void;
	loadSync: (request: AssetRequest) => Asset$1;
	setAssetManifest: (newManifest: AssetManifest) => void;
	resolveAsset: (assetPath: string) => string | null;
	getStatistics: () => AssetStatistics;
};
type AssetStatistics = {
	pending: number;
	complete: number;
	total: number;
};
type AssetRequest = {
	id?: Eid;
	url: string;
};
export declare const assets: AssetManager;
export declare const audio: {
	getCurrentTime: (world: World, eid: Eid) => number;
	setCurrentTime: (world: World, eid: Eid, time: number) => void;
};
export declare const PositionAnimation: RootAttribute<BaseSchema<{
	autoFrom: "boolean";
	fromX: "f32";
	fromY: "f32";
	fromZ: "f32";
	toX: "f32";
	toY: "f32";
	toZ: "f32";
	duration: "f32";
	loop: "boolean";
	reverse: "boolean";
	easeIn: "boolean";
	easeOut: "boolean";
	easingFunction: "string";
	target: "eid";
}>>;
export declare const ScaleAnimation: RootAttribute<BaseSchema<{
	autoFrom: "boolean";
	fromX: "f32";
	fromY: "f32";
	fromZ: "f32";
	toX: "f32";
	toY: "f32";
	toZ: "f32";
	duration: "f32";
	loop: "boolean";
	reverse: "boolean";
	easeIn: "boolean";
	easeOut: "boolean";
	easingFunction: "string";
	target: "eid";
}>>;
export declare const RotateAnimation: RootAttribute<BaseSchema<{
	autoFrom: "boolean";
	fromX: "f32";
	fromY: "f32";
	fromZ: "f32";
	toX: "f32";
	toY: "f32";
	toZ: "f32";
	shortestPath: "boolean";
	duration: "f32";
	loop: "boolean";
	reverse: "boolean";
	easeIn: "boolean";
	easeOut: "boolean";
	easingFunction: "string";
	target: "eid";
}>>;
export declare const CustomVec3Animation: RootAttribute<BaseSchema<{
	attribute: "string";
	autoFrom: "boolean";
	fromX: "f32";
	fromY: "f32";
	fromZ: "f32";
	toX: "f32";
	toY: "f32";
	toZ: "f32";
	duration: "f32";
	loop: "boolean";
	reverse: "boolean";
	easeIn: "boolean";
	easeOut: "boolean";
	easingFunction: "string";
	target: "eid";
}>>;
export declare const CustomPropertyAnimation: RootAttribute<BaseSchema<{
	attribute: "string";
	property: "string";
	autoFrom: "boolean";
	from: "f32";
	to: "f32";
	duration: "f32";
	loop: "boolean";
	reverse: "boolean";
	easeIn: "boolean";
	easeOut: "boolean";
	easingFunction: "string";
	target: "eid";
}>>;
export declare const FollowAnimation: RootAttribute<BaseSchema<{
	target: "eid";
	minDistance: "f32";
	maxDistance: "f32";
	elasticity: "f32";
}>>;
export declare const LookAtAnimation: RootAttribute<BaseSchema<{
	target: "eid";
	targetX: "f32";
	targetY: "f32";
	targetZ: "f32";
	lockX: "boolean";
	lockY: "boolean";
}>>;
export interface ParticlesSchema {
	stopped: boolean;
	emitterLife: number;
	particlesPerShot: number;
	emitDelay: number;
	minimumLifespan: number;
	maximumLifespan: number;
	mass: number;
	gravity: number;
	scale: number;
	forceX: number;
	forceY: number;
	forceZ: number;
	spread: number;
	radialVelocity: number;
	spawnAreaType: string;
	spawnAreaWidth: number;
	spawnAreaHeight: number;
	spawnAreaDepth: number;
	spawnAreaRadius: number;
	boundingZoneType: string;
	boundingZoneWidth: number;
	boundingZoneHeight: number;
	boundingZoneDepth: number;
	boundingZoneRadius: number;
	resourceType: string;
	resourceUrl: string;
	blendingMode: string;
	animateColor: boolean;
	colorStart: string;
	colorEnd: string;
	randomDrift: boolean;
	randomDriftRange: number;
	collisions: boolean;
}
export declare const ParticleEmitter: RootAttribute<BaseSchema<{
	stopped: "boolean";
	emitterLife: "f32";
	particlesPerShot: "ui32";
	emitDelay: "f32";
	minimumLifespan: "f32";
	maximumLifespan: "f32";
	mass: "f32";
	gravity: "f32";
	scale: "f32";
	forceX: "f32";
	forceY: "f32";
	forceZ: "f32";
	spread: "f32";
	radialVelocity: "f32";
	spawnAreaType: "string";
	spawnAreaWidth: "f32";
	spawnAreaHeight: "f32";
	spawnAreaDepth: "f32";
	spawnAreaRadius: "f32";
	boundingZoneType: "string";
	boundingZoneWidth: "f32";
	boundingZoneHeight: "f32";
	boundingZoneDepth: "f32";
	boundingZoneRadius: "f32";
	resourceType: "string";
	resourceUrl: "string";
	blendingMode: "string";
	animateColor: "boolean";
	colorStart: "string";
	colorEnd: "string";
	randomDrift: "boolean";
	randomDriftRange: "f32";
	collisions: "boolean";
}>>;
export declare const OrbitControls: RootAttribute<BaseSchema<{
	speed: "f32";
	maxAngularSpeed: "f32";
	maxZoomSpeed: "f32";
	distanceMin: "f32";
	distanceMax: "f32";
	pitchAngleMin: "f32";
	pitchAngleMax: "f32";
	constrainYaw: "boolean";
	yawAngleMin: "f32";
	yawAngleMax: "f32";
	inertiaFactor: "f32";
	focusEntity: "eid";
	invertedX: "boolean";
	invertedY: "boolean";
	invertedZoom: "boolean";
	controllerSupport: "boolean";
	horizontalSensitivity: "f32";
	verticalSensitivity: "f32";
}>>;
export declare const FlyController: RootAttribute<BaseSchema<{
	verticalSensitivity: "f32";
	horizontalSensitivity: "f32";
	moveSpeedX: "f32";
	moveSpeedY: "f32";
	moveSpeedZ: "f32";
	invertedY: "boolean";
	invertedX: "boolean";
}>>;
export declare const ready: () => Promise<void>;
export declare const isReady: () => boolean;
export declare const FaceAnchor: RootAttribute<BaseSchema<ExtendedSchema<Schema>>>;
export declare const FaceMeshAnchor: RootAttribute<BaseSchema<ExtendedSchema<Schema>>>;
export declare const FaceAttachment: RootAttribute<BaseSchema<{
	point: "string";
}>>;
export declare const Disabled: RootAttribute<{}>;
type EcsTextureKey = "textureSrc" | "roughnessMap" | "metalnessMap" | "normalMap" | "opacityMap" | "emissiveMap";
type VideoQuery = {
	src?: string;
	textureKey?: EcsTextureKey;
};
type VideoTimeResult = {
	src: string;
	textureKey: EcsTextureKey;
	time: number;
};
export declare const video: {
	getCurrentTime: (world: World, eid: Eid, query?: VideoQuery) => number;
	setCurrentTime: (world: World, eid: Eid, time: number, query?: VideoQuery) => void;
	getCurrentTimes: (world: World, eid: Eid, filter?: VideoQuery) => VideoTimeResult[];
	setCurrentTimes: (world: World, eid: Eid, time: number, filter?: VideoQuery) => void;
};
export type Ecs = typeof api;

declare namespace math {
	export { Mat4, Mat4Factory, Quat, QuatFactory, QuatSource, Vec2, Vec2Factory, Vec2Source, Vec3, Vec3Factory, Vec3Source, mat4, quat, vec2, vec3 };
}

export {
	Audio$1 as Audio,
	BoxGeometry$1 as BoxGeometry,
	Camera$2 as Camera,
	CapsuleGeometry$1 as CapsuleGeometry,
	CircleGeometry$1 as CircleGeometry,
	Collider$1 as Collider,
	ColliderType$1 as ColliderType,
	ConeGeometry$1 as ConeGeometry,
	CylinderGeometry$1 as CylinderGeometry,
	Face$1 as Face,
	FaceGeometry$1 as FaceGeometry,
	GltfModel$1 as GltfModel,
	HiderMaterial$1 as HiderMaterial,
	ImageTarget$1 as ImageTarget,
	Light$1 as Light,
	Material$2 as Material,
	PlaneGeometry$1 as PlaneGeometry,
	PolyhedronGeometry$1 as PolyhedronGeometry,
	Quaternion$1 as Quaternion,
	RingGeometry$1 as RingGeometry,
	Shadow$1 as Shadow,
	ShadowMaterial$1 as ShadowMaterial,
	SphereGeometry$1 as SphereGeometry,
	Splat$1 as Splat,
	TetrahedronGeometry$1 as TetrahedronGeometry,
	TorusGeometry$1 as TorusGeometry,
	UnlitMaterial$1 as UnlitMaterial,
	VideoMaterial$1 as VideoMaterial,
	math,
};

export {};

