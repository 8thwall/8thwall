/* eslint-disable */

/*!*********************************************************************
 * This Source Code Form is copyright of 51 Degrees Mobile Experts Limited.
 * Copyright 2022 51 Degrees Mobile Experts Limited, Davidson House,
 * Forbury Square, Reading, Berkshire, United Kingdom RG1 3EU.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0.
 *
 * If a copy of the MPL was not distributed with this file, You can obtain
 * one at http://mozilla.org/MPL/2.0/.
 *
 * This Source Code Form is "Incompatible With Secondary Licenses", as
 * defined by the Mozilla Public License, v. 2.0.
 ********************************************************************** */
  
'use strict';

// Interogates the device using JavaScript to work out the renderer.
// The takUrl parameter provides the URL of the tak.js file in a location that
// supports same-orgin operation with web workers. If the parameter is not 
// provided then CPU benchmarking will not be available to the detection 
// process.
// @param complete the function to call with the renderer.
// @param takUrl the full URL of the tak.js file (optional).
function getRenderer(complete, takUrl) {
	/*! VERSION = 1.657942 */ 
	var decisionTree = 
		{
		  "Version": "1.657942",
		  "PublishDate": "2023-10-05T13:31:50.4807708Z",
		  "Data": [
		    {
		      "x": "Unknown",
		      "m": function(n){return family(n);},
		      "n": [
		        108,
		        3,
		        2,
		        1
		      ]
		    },
		    {
		      "x": "Apple A9X GPU|Apple A10X GPU|Apple A9 GPU|Apple A10 GPU|Apple A11 GPU|Apple A12X GPU|Apple A12 GPU|Apple A8 GPU|Apple A8X GPU|Apple A13 GPU|Apple A14 GPU|Apple M1 GPU|Apple A12Z GPU|Apple A15 GPU|Apple A7 GPU|Apple A16 GPU|Apple M2 GPU|Apple A17 Pro GPU",
		      "m": function(n){return height(n);},
		      "n": [
		        109,
		        63,
		        60,
		        61,
		        62,
		        47,
		        45,
		        46,
		        33,
		        36,
		        32,
		        34,
		        35,
		        23,
		        18,
		        19,
		        11,
		        6,
		        7,
		        5
		      ],
		      "v": [
		        "Macintosh"
		      ]
		    },
		    {
		      "x": "Apple A7 GPU|Apple A8 GPU|Apple A9 GPU|Apple A10 GPU|Apple A11 GPU|Apple A12 GPU|Apple A13 GPU|Apple A14 GPU|Apple A15 GPU|Apple A16 GPU|Apple A17 Pro GPU",
		      "m": function(n){return height(n);},
		      "n": [
		        60,
		        61,
		        62,
		        44,
		        45,
		        46,
		        30,
		        31,
		        23,
		        17,
		        12,
		        6,
		        7
		      ],
		      "v": [
		        "iPhone"
		      ]
		    },
		    {
		      "x": "Apple A7 GPU|Apple A8 GPU|Apple A9X GPU|Apple A10X GPU|Apple A9 GPU|Apple A12X GPU|Apple A10 GPU|Apple A12 GPU|Apple A8X GPU|Apple M1 GPU|Apple A14 GPU|Apple A12Z GPU|Apple A15 GPU|Apple A13 GPU|Apple M2 GPU",
		      "m": function(n){return height(n);},
		      "n": [
		        109,
		        110,
		        63,
		        47,
		        33,
		        32,
		        18,
		        4
		      ],
		      "v": [
		        "iPad"
		      ]
		    },
		    {
		      "x": "Apple A7 GPU|Apple A8 GPU|Apple A9X GPU|Apple A10X GPU|Apple A9 GPU|Apple A12X GPU|Apple A10 GPU|Apple A12 GPU|Apple A8X GPU|Apple M1 GPU|Apple A12Z GPU|Apple M2 GPU",
		      "m": function(n){return mediacolorgamut(n);},
		      "n": [
		        16,
		        13
		      ],
		      "v": [
		        2048
		      ]
		    },
		    {
		      "x": "Apple A9X GPU|Apple A10X GPU|Apple A9 GPU|Apple A12X GPU|Apple A10 GPU|Apple A12 GPU|Apple A8 GPU|Apple A8X GPU|Apple M1 GPU|Apple A12Z GPU|Apple A7 GPU|Apple M2 GPU",
		      "m": function(n){return mediacolorgamut(n);},
		      "n": [
		        16,
		        14
		      ],
		      "v": [
		        2048
		      ]
		    },
		    {
		      "x": "Apple A11 GPU|Apple A12 GPU|Apple A13 GPU|Apple A14 GPU|Apple A15 GPU|Apple A16 GPU|Apple A17 Pro GPU",
		      "m": function(n){return hash3d(n);},
		      "n": [
		        114,
		        73,
		        74,
		        72,
		        75,
		        21,
		        26,
		        20,
		        8
		      ],
		      "v": [
		        2436
		      ]
		    },
		    {
		      "x": "Apple A11 GPU|Apple A12 GPU|Apple A14 GPU|Apple A13 GPU|Apple A15 GPU|Apple A16 GPU|Apple A17 Pro GPU",
		      "m": function(n){return hash3d(n);},
		      "n": [
		        121,
		        120,
		        115,
		        50,
		        27,
		        28,
		        21,
		        9
		      ],
		      "v": [
		        2079
		      ]
		    },
		    {
		      "x": "Apple A16 GPU|Apple A17 Pro GPU|Apple A14 GPU|Apple A15 GPU|Apple A12 GPU|Apple A13 GPU",
		      "m": function(n){return hash(n);},
		      "n": [
		        43,
		        15
		      ],
		      "v": [
		        3711606621
		      ]
		    },
		    {
		      "x": "Apple A17 Pro GPU|Apple A16 GPU|Apple A15 GPU|Apple A14 GPU|Apple A13 GPU|Apple A12 GPU",
		      "m": function(n){return hash(n);},
		      "n": [
		        103,
		        10
		      ],
		      "v": [
		        3711606621
		      ]
		    },
		    {
		      "x": "Apple A17 Pro GPU|Apple A16 GPU|Apple A15 GPU|Apple A14 GPU",
		      "v": [
		        235283973
		      ]
		    },
		    {
		      "x": "Apple A9 GPU|Apple A10 GPU|Apple A11 GPU|Apple A13 GPU|Apple A15 GPU|Apple A7 GPU|Apple A8 GPU",
		      "m": function(n){return mediacolorgamut(n);},
		      "n": [
		        38,
		        25
		      ],
		      "v": [
		        1136
		      ]
		    },
		    {
		      "x": "Apple A7 GPU|Apple A9 GPU|Apple A10 GPU|Apple A11 GPU|Apple A8 GPU|Apple A13 GPU|Apple A15 GPU",
		      "m": function(n){return mediacolorgamut(n);},
		      "n": [
		        37,
		        38
		      ],
		      "v": [
		        1136
		      ]
		    },
		    {
		      "x": "Apple A7 GPU|Apple A8 GPU|Apple A9X GPU|Apple A9 GPU|Apple A10 GPU|Apple A8X GPU",
		      "m": function(n){return hash(n);},
		      "n": [
		        149,
		        148,
		        89,
		        90,
		        91,
		        92,
		        55,
		        56,
		        58,
		        54,
		        57
		      ],
		      "v": [
		        "srgb"
		      ]
		    },
		    {
		      "x": "Apple A9X GPU|Apple A9 GPU|Apple A10 GPU|Apple A8 GPU|Apple A8X GPU|Apple A7 GPU",
		      "m": function(n){return hash(n);},
		      "n": [
		        149,
		        153,
		        97,
		        98,
		        92,
		        55,
		        56,
		        58,
		        54
		      ],
		      "v": [
		        "srgb"
		      ]
		    },
		    {
		      "x": "Apple A16 GPU|Apple A17 Pro GPU|Apple A14 GPU|Apple A15 GPU",
		      "v": [
		        235283973
		      ]
		    },
		    {
		      "x": "Apple A10X GPU|Apple A9X GPU|Apple A12X GPU|Apple A12 GPU|Apple M1 GPU|Apple A12Z GPU|Apple M2 GPU",
		      "m": function(n){return hash3d(n);},
		      "n": [
		        150,
		        126,
		        127,
		        94,
		        93,
		        95,
		        79
		      ],
		      "v": [
		        "p3"
		      ]
		    },
		    {
		      "x": "Apple A8 GPU|Apple A9 GPU|Apple A10 GPU|Apple A11 GPU|Apple A13 GPU|Apple A15 GPU",
		      "m": function(n){return mediacolorgamut(n);},
		      "n": [
		        68,
		        39
		      ],
		      "v": [
		        1334
		      ]
		    },
		    {
		      "x": "Apple A9X GPU|Apple A10X GPU|Apple A12X GPU|Apple M1 GPU|Apple A12Z GPU|Apple M2 GPU",
		      "m": function(n){return mediacolorgamut(n);},
		      "n": [
		        111,
		        24
		      ],
		      "v": [
		        2732
		      ]
		    },
		    {
		      "x": "Apple A9 GPU|Apple A10 GPU|Apple A11 GPU|Apple A13 GPU|Apple A15 GPU|Apple A8 GPU",
		      "m": function(n){return mediacolorgamut(n);},
		      "n": [
		        70,
		        39
		      ],
		      "v": [
		        1334
		      ]
		    },
		    {
		      "x": "Apple A14 GPU|Apple A16 GPU|Apple A15 GPU|Apple A13 GPU",
		      "m": function(n){return hash(n);},
		      "n": [
		        157,
		        100,
		        42
		      ],
		      "v": [
		        3403189785
		      ]
		    },
		    {
		      "x": "Apple A14 GPU|Apple A15 GPU",
		      "m": function(n){return hash(n);},
		      "n": [
		        22
		      ],
		      "v": [
		        2364051618
		      ]
		    },
		    {
		      "x": "Apple A15 GPU|Apple A14 GPU",
		      "v": [
		        2775654583
		      ]
		    },
		    {
		      "x": "Apple A14 GPU|Apple A15 GPU",
		      "m": function(n){return hash3d(n);},
		      "n": [
		        74,
		        51,
		        40,
		        21,
		        29
		      ],
		      "v": [
		        2532
		      ]
		    },
		    {
		      "x": "Apple A10X GPU|Apple A12X GPU|Apple M1 GPU|Apple A12Z GPU|Apple M2 GPU",
		      "m": function(n){return hash3d(n);},
		      "n": [
		        152,
		        151,
		        150,
		        126,
		        127,
		        41
		      ],
		      "v": [
		        "p3"
		      ]
		    },
		    {
		      "x": "Apple A9 GPU|Apple A10 GPU|Apple A7 GPU|Apple A8 GPU",
		      "m": function(n){return hash3d(n);},
		      "n": [
		        155,
		        156,
		        99
		      ],
		      "v": [
		        "srgb"
		      ]
		    },
		    {
		      "x": "Apple A14 GPU|Apple A16 GPU|Apple A15 GPU",
		      "v": [
		        46273595
		      ]
		    },
		    {
		      "x": "Apple A14 GPU|Apple A15 GPU|Apple A16 GPU|Apple A13 GPU",
		      "m": function(n){return hash(n);},
		      "n": [
		        157,
		        101,
		        102
		      ],
		      "v": [
		        3403189785
		      ]
		    },
		    {
		      "x": "Apple A15 GPU|Apple A16 GPU|Apple A14 GPU",
		      "v": [
		        46273595
		      ]
		    },
		    {
		      "x": "Apple A15 GPU|Apple A14 GPU",
		      "v": [
		        3711606621
		      ]
		    },
		    {
		      "x": "Apple A8 GPU|Apple A10 GPU|Apple A11 GPU|Apple A9 GPU",
		      "m": function(n){return mediacolorgamut(n);},
		      "n": [
		        64,
		        65
		      ],
		      "v": [
		        2001
		      ]
		    },
		    {
		      "x": "Apple A8 GPU|Apple A9 GPU|Apple A10 GPU|Apple A11 GPU",
		      "m": function(n){return mediacolorgamut(n);},
		      "n": [
		        66,
		        67
		      ],
		      "v": [
		        2208
		      ]
		    },
		    {
		      "x": "Apple A12X GPU|Apple M1 GPU|Apple A12Z GPU|Apple M2 GPU",
		      "m": function(n){return hash3d(n);},
		      "n": [
		        126,
		        127,
		        79,
		        78
		      ],
		      "v": [
		        2388
		      ]
		    },
		    {
		      "x": "Apple A14 GPU|Apple M1 GPU",
		      "m": function(n){return mediacolorgamut(n);},
		      "n": [
		        112,
		        48
		      ],
		      "v": [
		        2360
		      ]
		    },
		    {
		      "x": "Apple A9 GPU|Apple A10 GPU|Apple A11 GPU|Apple A8 GPU",
		      "m": function(n){return mediacolorgamut(n);},
		      "n": [
		        69,
		        67
		      ],
		      "v": [
		        2208
		      ]
		    },
		    {
		      "x": "Apple A10 GPU|Apple A11 GPU|Apple A9 GPU|Apple A8 GPU",
		      "m": function(n){return mediacolorgamut(n);},
		      "n": [
		        71,
		        65
		      ],
		      "v": [
		        2001
		      ]
		    },
		    {
		      "x": "Apple A14 GPU|Apple A15 GPU|Apple M2 GPU",
		      "m": function(n){return ratio(n);},
		      "n": [
		        113,
		        49
		      ],
		      "v": [
		        2778
		      ]
		    },
		    {
		      "x": "Apple A7 GPU|Apple A9 GPU|Apple A8 GPU",
		      "m": function(n){return hash(n);},
		      "n": [
		        130,
		        131,
		        82,
		        83,
		        84
		      ],
		      "v": [
		        "srgb"
		      ]
		    },
		    {
		      "x": "Apple A10 GPU|Apple A11 GPU|Apple A13 GPU|Apple A15 GPU",
		      "m": function(n){return hash3d(n);},
		      "n": [
		        132,
		        133,
		        118,
		        134,
		        85,
		        86
		      ],
		      "v": [
		        "p3"
		      ]
		    },
		    {
		      "x": "Apple A10 GPU|Apple A11 GPU|Apple A13 GPU|Apple A15 GPU",
		      "m": function(n){return hash(n);},
		      "n": [
		        144,
		        145,
		        147,
		        146,
		        87,
		        88
		      ],
		      "v": [
		        "p3"
		      ]
		    },
		    {
		      "x": "Apple A14 GPU|Apple A15 GPU",
		      "m": function(n){return hash(n);},
		      "n": [
		        100,
		        104
		      ],
		      "v": [
		        3403189785
		      ]
		    },
		    {
		      "x": "Apple M1 GPU|Apple A10X GPU|Apple A12Z GPU|Apple M2 GPU",
		      "m": function(n){return hash(n);},
		      "n": [
		        174,
		        107,
		        159,
		        106
		      ],
		      "v": [
		        3403189785
		      ]
		    },
		    {
		      "x": "Apple A14 GPU|Apple A16 GPU|Apple A15 GPU",
		      "v": [
		        2775654583
		      ]
		    },
		    {
		      "x": "Apple A12 GPU|Apple A13 GPU",
		      "v": [
		        3565683531
		      ]
		    },
		    {
		      "x": "Apple A14 GPU|Apple A15 GPU",
		      "m": function(n){return hash3d(n);},
		      "n": [
		        74,
		        122,
		        77,
		        52,
		        29
		      ],
		      "v": [
		        2778
		      ]
		    },
		    {
		      "x": "Apple A16 GPU|Apple A17 Pro GPU",
		      "m": function(n){return hash3d(n);},
		      "n": [
		        123
		      ],
		      "v": [
		        2796
		      ]
		    },
		    {
		      "x": "Apple A16 GPU|Apple A17 Pro GPU",
		      "m": function(n){return hash3d(n);},
		      "n": [
		        123,
		        53
		      ],
		      "v": [
		        2556
		      ]
		    },
		    {
		      "x": "Apple A10 GPU|Apple A12 GPU|Apple A13 GPU",
		      "m": function(n){return hash(n);},
		      "n": [
		        128,
		        129,
		        80,
		        81
		      ],
		      "v": [
		        2160
		      ]
		    },
		    {
		      "x": "Apple A14 GPU|Apple M1 GPU",
		      "m": function(n){return hash3d(n);},
		      "n": [
		        74,
		        96,
		        59
		      ],
		      "v": [
		        "p3"
		      ]
		    },
		    {
		      "x": "Apple A14 GPU|Apple A15 GPU",
		      "m": function(n){return hash3d(n);},
		      "n": [
		        74,
		        122,
		        77,
		        52,
		        29
		      ],
		      "v": [
		        3.0
		      ]
		    },
		    {
		      "x": "Apple A14 GPU",
		      "v": [
		        105985484,
		        679860869
		      ]
		    },
		    {
		      "x": "Apple A15 GPU",
		      "v": [
		        46273595,
		        679860869
		      ]
		    },
		    {
		      "x": "Apple A14 GPU|Apple A15 GPU",
		      "m": function(n){return hash(n);},
		      "n": [
		        22
		      ],
		      "v": [
		        3403189785
		      ]
		    },
		    {
		      "x": "Apple A17 Pro GPU|Apple A16 GPU",
		      "v": [
		        3711606621
		      ]
		    },
		    {
		      "x": "Apple A7 GPU",
		      "v": [
		        1915583345
		      ]
		    },
		    {
		      "x": "Apple A9X GPU|Apple A9 GPU|Apple A10 GPU",
		      "v": [
		        3129316290,
		        3249312110
		      ]
		    },
		    {
		      "x": "Apple A9 GPU|Apple A9X GPU|Apple A10 GPU",
		      "m": function(n){return hash3d(n);},
		      "n": [
		        168,
		        105
		      ],
		      "v": [
		        2114570256
		      ]
		    },
		    {
		      "x": "Apple A7 GPU",
		      "v": [
		        857422828
		      ]
		    },
		    {
		      "x": "Apple A9X GPU|Apple A9 GPU|Apple A10 GPU",
		      "m": function(n){return hash3d(n);},
		      "n": [
		        171
		      ],
		      "v": [
		        63583436
		      ]
		    },
		    {
		      "x": "Apple A14 GPU|Apple M1 GPU",
		      "m": function(n){return hash(n);},
		      "n": [
		        175,
		        107
		      ],
		      "v": [
		        3403189785
		      ]
		    },
		    {
		      "x": "Apple A12 GPU|Apple A13 GPU",
		      "m": function(n){return hash3d(n);},
		      "n": [
		        116,
		        115,
		        76
		      ],
		      "v": [
		        2688
		      ]
		    },
		    {
		      "x": "Apple A12 GPU|Apple A13 GPU",
		      "m": function(n){return hash3d(n);},
		      "n": [
		        118,
		        117,
		        76
		      ],
		      "v": [
		        1624
		      ]
		    },
		    {
		      "x": "Apple A12 GPU|Apple A13 GPU",
		      "m": function(n){return hash3d(n);},
		      "n": [
		        118,
		        119,
		        76
		      ],
		      "v": [
		        1792
		      ]
		    },
		    {
		      "x": "Apple A10X GPU|Apple A12 GPU",
		      "m": function(n){return hash(n);},
		      "n": [
		        125,
		        124
		      ],
		      "v": [
		        2224
		      ]
		    },
		    {
		      "x": "Apple A8 GPU|Apple A9 GPU",
		      "m": function(n){return hash(n);},
		      "n": [
		        135,
		        136
		      ],
		      "v": [
		        "srgb"
		      ]
		    },
		    {
		      "x": "Apple A10 GPU|Apple A11 GPU",
		      "m": function(n){return hash(n);},
		      "n": [
		        137,
		        138
		      ],
		      "v": [
		        "p3"
		      ]
		    },
		    {
		      "x": "Apple A8 GPU|Apple A9 GPU",
		      "m": function(n){return hash(n);},
		      "n": [
		        139,
		        140
		      ],
		      "v": [
		        "srgb"
		      ]
		    },
		    {
		      "x": "Apple A10 GPU|Apple A11 GPU",
		      "m": function(n){return hash(n);},
		      "n": [
		        137,
		        141
		      ],
		      "v": [
		        "p3"
		      ]
		    },
		    {
		      "x": "Apple A8 GPU|Apple A9 GPU",
		      "m": function(n){return hash(n);},
		      "n": [
		        142,
		        143
		      ],
		      "v": [
		        "srgb"
		      ]
		    },
		    {
		      "x": "Apple A9 GPU|Apple A8 GPU",
		      "m": function(n){return hash(n);},
		      "n": [
		        154,
		        140
		      ],
		      "v": [
		        "srgb"
		      ]
		    },
		    {
		      "x": "Apple A9 GPU|Apple A8 GPU",
		      "m": function(n){return hash(n);},
		      "n": [
		        154,
		        143
		      ],
		      "v": [
		        "srgb"
		      ]
		    },
		    {
		      "x": "Apple A9 GPU|Apple A8 GPU",
		      "m": function(n){return hash(n);},
		      "n": [
		        154,
		        136
		      ],
		      "v": [
		        "srgb"
		      ]
		    },
		    {
		      "x": "Apple A12 GPU",
		      "v": [
		        958581112,
		        4085158452
		      ]
		    },
		    {
		      "x": "Apple A13 GPU",
		      "v": [
		        1278953537,
		        3335845976,
		        4193218782
		      ]
		    },
		    {
		      "x": "Apple A14 GPU",
		      "v": [
		        105985484
		      ]
		    },
		    {
		      "x": "Apple A12 GPU",
		      "v": [
		        2301174800
		      ]
		    },
		    {
		      "x": "Apple A13 GPU|Apple A12 GPU",
		      "v": [
		        3711606621
		      ]
		    },
		    {
		      "x": "Apple A14 GPU|Apple A15 GPU",
		      "m": function(n){return hash(n);},
		      "n": [
		        104
		      ],
		      "v": [
		        2364051618
		      ]
		    },
		    {
		      "x": "Apple A12X GPU|Apple A12Z GPU",
		      "v": [
		        4085158452
		      ]
		    },
		    {
		      "x": "Apple M1 GPU|Apple M2 GPU",
		      "m": function(n){return hash(n);},
		      "n": [
		        158,
		        159
		      ],
		      "v": [
		        3403189785
		      ]
		    },
		    {
		      "x": "Apple A12 GPU|Apple A13 GPU",
		      "m": function(n){return hash3d(n);},
		      "n": [
		        160,
		        75
		      ],
		      "v": [
		        2206992415
		      ]
		    },
		    {
		      "x": "Apple A13 GPU|Apple A12 GPU",
		      "m": function(n){return hash3d(n);},
		      "n": [
		        160,
		        75
		      ],
		      "v": [
		        2866949877
		      ]
		    },
		    {
		      "x": "Apple A9 GPU",
		      "v": [
		        46663968,
		        3129316290
		      ]
		    },
		    {
		      "x": "Apple A9 GPU",
		      "v": [
		        2114570256
		      ]
		    },
		    {
		      "x": "Apple A9 GPU",
		      "v": [
		        63583436
		      ]
		    },
		    {
		      "x": "Apple A13 GPU|Apple A15 GPU",
		      "m": function(n){return hash(n);},
		      "n": [
		        161,
		        162
		      ],
		      "v": [
		        3403189785
		      ]
		    },
		    {
		      "x": "Apple A13 GPU|Apple A15 GPU",
		      "m": function(n){return hash(n);},
		      "n": [
		        163,
		        164
		      ],
		      "v": [
		        3711606621
		      ]
		    },
		    {
		      "x": "Apple A11 GPU|Apple A13 GPU",
		      "m": function(n){return hash3d(n);},
		      "n": [
		        165,
		        166
		      ],
		      "v": [
		        1349146759
		      ]
		    },
		    {
		      "x": "Apple A13 GPU|Apple A11 GPU",
		      "m": function(n){return hash3d(n);},
		      "n": [
		        167,
		        160
		      ],
		      "v": [
		        2206992415
		      ]
		    },
		    {
		      "x": "Apple A8 GPU|Apple A8X GPU",
		      "v": [
		        1361285941,
		        3816812018,
		        4125234388
		      ]
		    },
		    {
		      "x": "Apple A8 GPU|Apple A8X GPU",
		      "m": function(n){return hash3d(n);},
		      "n": [
		        169,
		        170
		      ],
		      "v": [
		        4005673483
		      ]
		    },
		    {
		      "x": "Apple A8 GPU|Apple A8X GPU",
		      "m": function(n){return hash3d(n);},
		      "n": [
		        169
		      ],
		      "v": [
		        1350183384
		      ]
		    },
		    {
		      "x": "Apple A8 GPU|Apple A8X GPU",
		      "m": function(n){return hash3d(n);},
		      "n": [
		        173,
		        172
		      ],
		      "v": [
		        2870741841
		      ]
		    },
		    {
		      "x": "Apple A10X GPU|Apple A9X GPU",
		      "v": [
		        583354101,
		        3458129248
		      ]
		    },
		    {
		      "x": "Apple A12X GPU|Apple A12 GPU",
		      "v": [
		        4085158452
		      ]
		    },
		    {
		      "x": "Apple A10X GPU|Apple A9X GPU",
		      "v": [
		        3928876783
		      ]
		    },
		    {
		      "x": "Apple M1 GPU",
		      "v": [
		        2364051618
		      ]
		    },
		    {
		      "x": "Apple A8 GPU|Apple A8X GPU",
		      "m": function(n){return hash3d(n);},
		      "n": [
		        176,
		        170
		      ],
		      "v": [
		        4005673483
		      ]
		    },
		    {
		      "x": "Apple A8 GPU|Apple A8X GPU",
		      "v": [
		        1361285941
		      ]
		    },
		    {
		      "x": "Apple A9 GPU",
		      "v": [
		        583354101,
		        3403189785,
		        3458129248,
		        3928876783
		      ]
		    },
		    {
		      "x": "Apple A14 GPU",
		      "v": [
		        1349146759,
		        1444462398
		      ]
		    },
		    {
		      "x": "Apple A14 GPU",
		      "v": [
		        1444462398
		      ]
		    },
		    {
		      "x": "Apple A15 GPU|Apple A16 GPU",
		      "v": [
		        2775654583
		      ]
		    },
		    {
		      "x": "Apple A13 GPU|Apple A12 GPU",
		      "v": [
		        3565683531
		      ]
		    },
		    {
		      "x": "Apple A15 GPU",
		      "v": [
		        2775654583
		      ]
		    },
		    {
		      "x": "Apple A9X GPU|Apple A10 GPU",
		      "v": [
		        3458129248
		      ]
		    },
		    {
		      "x": "Apple M1 GPU|Apple A12Z GPU",
		      "v": [
		        1349146759
		      ]
		    },
		    {
		      "x": "Apple M1 GPU",
		      "v": [
		        1444462398
		      ]
		    },
		    {
		      "x": "Apple A10 GPU",
		      "v": [
		        "iPod Touch"
		      ]
		    },
		    {
		      "x": "Apple A15 GPU",
		      "v": [
		        2266
		      ]
		    },
		    {
		      "x": "Apple M2 GPU",
		      "v": [
		        2778
		      ]
		    },
		    {
		      "x": "Apple A9X GPU",
		      "v": [
		        "srgb"
		      ]
		    },
		    {
		      "x": "Apple A14 GPU",
		      "v": [
		        "srgb"
		      ]
		    },
		    {
		      "x": "Apple M2 GPU",
		      "v": [
		        2.0
		      ]
		    },
		    {
		      "x": "Apple A11 GPU",
		      "v": [
		        367695777,
		        411650080,
		        1220644697
		      ]
		    },
		    {
		      "x": "Apple A12 GPU",
		      "v": [
		        958581112,
		        2301174800,
		        4085158452
		      ]
		    },
		    {
		      "x": "Apple A13 GPU",
		      "v": [
		        352823931,
		        1278953537,
		        3335845976,
		        4193218782
		      ]
		    },
		    {
		      "x": "Apple A12 GPU",
		      "v": [
		        0,
		        958581112,
		        2301174800,
		        3403189785,
		        4085158452
		      ]
		    },
		    {
		      "x": "Apple A13 GPU",
		      "v": [
		        352823931,
		        3335845976,
		        4193218782
		      ]
		    },
		    {
		      "x": "Apple A12 GPU",
		      "v": [
		        958581112,
		        2301174800,
		        3403189785,
		        4085158452
		      ]
		    },
		    {
		      "x": "Apple A11 GPU",
		      "v": [
		        367695777,
		        411650080
		      ]
		    },
		    {
		      "x": "Apple A13 GPU",
		      "v": [
		        352823931,
		        1278953537,
		        3335845976
		      ]
		    },
		    {
		      "x": "Apple A15 GPU",
		      "v": [
		        1407135659
		      ]
		    },
		    {
		      "x": "Apple A16 GPU",
		      "v": [
		        46273595,
		        3403189785
		      ]
		    },
		    {
		      "x": "Apple A10X GPU",
		      "v": [
		        63583436,
		        2114570256,
		        3129316290,
		        3249312110
		      ]
		    },
		    {
		      "x": "Apple A12 GPU",
		      "v": [
		        1349146759,
		        2917249763
		      ]
		    },
		    {
		      "x": "Apple M1 GPU",
		      "v": [
		        105985484,
		        2364051618
		      ]
		    },
		    {
		      "x": "Apple M2 GPU",
		      "v": [
		        46273595
		      ]
		    },
		    {
		      "x": "Apple A10 GPU",
		      "v": [
		        2114570256
		      ]
		    },
		    {
		      "x": "Apple A12 GPU",
		      "v": [
		        1349146759
		      ]
		    },
		    {
		      "x": "Apple A7 GPU",
		      "v": [
		        857422828,
		        1915583345
		      ]
		    },
		    {
		      "x": "Apple A8 GPU",
		      "v": [
		        839732043,
		        3816812018,
		        4125234388
		      ]
		    },
		    {
		      "x": "Apple A10 GPU",
		      "v": [
		        583354101,
		        3458129248,
		        3928876783
		      ]
		    },
		    {
		      "x": "Apple A11 GPU",
		      "v": [
		        367695777,
		        411650080,
		        1220644697,
		        1804407534
		      ]
		    },
		    {
		      "x": "Apple A15 GPU",
		      "v": [
		        2364051618
		      ]
		    },
		    {
		      "x": "Apple A8 GPU",
		      "v": [
		        1411440593,
		        1924197914,
		        4125234388
		      ]
		    },
		    {
		      "x": "Apple A9 GPU",
		      "v": [
		        2114570256,
		        3129316290
		      ]
		    },
		    {
		      "x": "Apple A10 GPU",
		      "v": [
		        63583436,
		        2114570256,
		        3129316290
		      ]
		    },
		    {
		      "x": "Apple A11 GPU",
		      "v": [
		        1349146759,
		        2206992415,
		        2917249763,
		        2946940121
		      ]
		    },
		    {
		      "x": "Apple A8 GPU",
		      "v": [
		        1411440593,
		        1913250432,
		        3074367344,
		        4125234388
		      ]
		    },
		    {
		      "x": "Apple A9 GPU",
		      "v": [
		        46663968,
		        2114570256,
		        3129316290
		      ]
		    },
		    {
		      "x": "Apple A11 GPU",
		      "v": [
		        2206992415,
		        2917249763,
		        2946940121,
		        3237505312
		      ]
		    },
		    {
		      "x": "Apple A8 GPU",
		      "v": [
		        3128296539,
		        3816812018,
		        4125234388
		      ]
		    },
		    {
		      "x": "Apple A9 GPU",
		      "v": [
		        46663968,
		        63583436,
		        2114570256,
		        3129316290
		      ]
		    },
		    {
		      "x": "Apple A10 GPU",
		      "v": [
		        46663968,
		        63583436,
		        2114570256,
		        3129316290
		      ]
		    },
		    {
		      "x": "Apple A11 GPU",
		      "v": [
		        2917249763,
		        2946940121,
		        3237505312
		      ]
		    },
		    {
		      "x": "Apple A15 GPU",
		      "v": [
		        235283973,
		        1444462398,
		        2775654583
		      ]
		    },
		    {
		      "x": "Apple A13 GPU",
		      "v": [
		        2866949877,
		        3565683531
		      ]
		    },
		    {
		      "x": "Apple A8 GPU",
		      "v": [
		        2656686317,
		        3710391565
		      ]
		    },
		    {
		      "x": "Apple A10 GPU",
		      "v": [
		        46663968
		      ]
		    },
		    {
		      "x": "Apple A12Z GPU",
		      "v": [
		        958581112,
		        2301174800,
		        2487400911
		      ]
		    },
		    {
		      "x": "Apple A12X GPU",
		      "v": [
		        4085158452
		      ]
		    },
		    {
		      "x": "Apple A10X GPU",
		      "v": [
		        583354101,
		        3458129248,
		        3928876783
		      ]
		    },
		    {
		      "x": "Apple A8X GPU",
		      "v": [
		        1350183384,
		        3816812018,
		        4125234388
		      ]
		    },
		    {
		      "x": "Apple A8 GPU",
		      "v": [
		        4125234388
		      ]
		    },
		    {
		      "x": "Apple A7 GPU",
		      "v": [
		        1966062736
		      ]
		    },
		    {
		      "x": "Apple A8 GPU",
		      "v": [
		        2998196247
		      ]
		    },
		    {
		      "x": "Apple A13 GPU",
		      "v": [
		        2866949877
		      ]
		    },
		    {
		      "x": "Apple M1 GPU",
		      "v": [
		        1349146759,
		        1444462398
		      ]
		    },
		    {
		      "x": "Apple M2 GPU",
		      "v": [
		        2775654583
		      ]
		    },
		    {
		      "x": "Apple A13 GPU",
		      "v": [
		        3335845976
		      ]
		    },
		    {
		      "x": "Apple A13 GPU",
		      "v": [
		        1349146759
		      ]
		    },
		    {
		      "x": "Apple A15 GPU",
		      "v": [
		        1444462398
		      ]
		    },
		    {
		      "x": "Apple A13 GPU",
		      "v": [
		        3565683531
		      ]
		    },
		    {
		      "x": "Apple A15 GPU",
		      "v": [
		        235283973
		      ]
		    },
		    {
		      "x": "Apple A11 GPU",
		      "v": [
		        411650080,
		        1220644697
		      ]
		    },
		    {
		      "x": "Apple A13 GPU",
		      "v": [
		        352823931,
		        3403189785,
		        4193218782
		      ]
		    },
		    {
		      "x": "Apple A11 GPU",
		      "v": [
		        367695777
		      ]
		    },
		    {
		      "x": "Apple A10 GPU",
		      "v": [
		        3403189785
		      ]
		    },
		    {
		      "x": "Apple A8X GPU",
		      "v": [
		        1783160115
		      ]
		    },
		    {
		      "x": "Apple A8 GPU",
		      "v": [
		        3928382683
		      ]
		    },
		    {
		      "x": "Apple A10 GPU",
		      "v": [
		        1058363647,
		        2015944978
		      ]
		    },
		    {
		      "x": "Apple A8 GPU",
		      "v": [
		        3312905059,
		        3928382683
		      ]
		    },
		    {
		      "x": "Apple A8X GPU",
		      "v": [
		        1480368425,
		        1783160115,
		        3403189785
		      ]
		    },
		    {
		      "x": "Apple A10X GPU",
		      "v": [
		        2114570256
		      ]
		    },
		    {
		      "x": "Apple A14 GPU",
		      "v": [
		        1349146759
		      ]
		    },
		    {
		      "x": "Apple A8X GPU",
		      "v": [
		        1783160115,
		        3403189785
		      ]
		    }
		  ]
		};

    function hash3d() {
        // This function is based on this article: https://bl.ocks.org/camargo/649e5903c4584a21a568972d4a2c16d3
        // The 'mat4' object is created from code in the gl-matrix library: https://github.com/toji/gl-matrix
        var gl, program, canvas;

        // The non-minified versions of these shaders are available in
        // WebSite/partials/fragment-shader.glsl and
        // WebSite/partials/vertex-shader.glsl
        var VERTEX_SHADER = 'attribute vec3 c,d; uniform vec4 e; uniform vec3 f,g;uniform mat4 h,i;varying vec3 j;void main(){vec3 a=normalize(d);vec4 b=h*vec4(c,1.);vec3 k=normalize(vec3(e-b));j=g*f*max(dot(k,a),0.),gl_Position=i*vec4(c,1.);}';
        var FRAGMENT_SHADER = '#ifdef GL_ES\n' +
            'precision mediump float;\n' +
            '#endif\n' +
            'varying vec3 j;void main(){gl_FragColor = vec4(j, 1.0);}';

        // This object uses code from the gl-matrix library: https://github.com/toji/gl-matrix
        var mat4 = {
            create: function () {
                var result = new Array(16);
                for (var i = 0; i < 16; i++) {
                    result[i] = (i % 5 == 0 ? 1 : 0);
                }
                return result;
            },
            perspective: function (out, fovy, aspect, near, far) {
                var f = 1.0 / Math.tan(fovy / 2), nf;
                out[0] = f / aspect;
                out[1] = 0;
                out[2] = 0;
                out[3] = 0;
                out[4] = 0;
                out[5] = f;
                out[6] = 0;
                out[7] = 0;
                out[8] = 0;
                out[9] = 0;
                out[11] = -1;
                out[12] = 0;
                out[13] = 0;
                out[15] = 0;
                if (far != null && far !== Infinity) {
                    nf = 1 / (near - far);
                    out[10] = (far + near) * nf;
                    out[14] = (2 * far * near) * nf;
                } else {
                    out[10] = -1;
                    out[14] = -2 * near;
                }
                return out;
            },
            lookAt: function (out, eye, center, up) {
                var x0, x1, x2, y0, y1, y2, z0, z1, z2, len;
                var eyex = eye[0];
                var eyey = eye[1];
                var eyez = eye[2];
                var upx = up[0];
                var upy = up[1];
                var upz = up[2];
                var centerx = center[0];
                var centery = center[1];
                var centerz = center[2];

                if (Math.abs(eyex - centerx) < 0.000001 &&
                    Math.abs(eyey - centery) < 0.000001 &&
                    Math.abs(eyez - centerz) < 0.000001) {
                    return mat4.identity(out);
                }

                z0 = eyex - centerx;
                z1 = eyey - centery;
                z2 = eyez - centerz;

                len = 1 / Math.hypot(z0, z1, z2);
                z0 *= len;
                z1 *= len;
                z2 *= len;

                x0 = upy * z2 - upz * z1;
                x1 = upz * z0 - upx * z2;
                x2 = upx * z1 - upy * z0;
                len = Math.hypot(x0, x1, x2);
                if (!len) {
                    x0 = 0;
                    x1 = 0;
                    x2 = 0;
                } else {
                    len = 1 / len;
                    x0 *= len;
                    x1 *= len;
                    x2 *= len;
                }

                y0 = z1 * x2 - z2 * x1;
                y1 = z2 * x0 - z0 * x2;
                y2 = z0 * x1 - z1 * x0;

                len = Math.hypot(y0, y1, y2);
                if (!len) {
                    y0 = 0;
                    y1 = 0;
                    y2 = 0;
                } else {
                    len = 1 / len;
                    y0 *= len;
                    y1 *= len;
                    y2 *= len;
                }

                out[0] = x0;
                out[1] = y0;
                out[2] = z0;
                out[3] = 0;
                out[4] = x1;
                out[5] = y1;
                out[6] = z1;
                out[7] = 0;
                out[8] = x2;
                out[9] = y2;
                out[10] = z2;
                out[11] = 0;
                out[12] = -(x0 * eyex + x1 * eyey + x2 * eyez);
                out[13] = -(y0 * eyex + y1 * eyey + y2 * eyez);
                out[14] = -(z0 * eyex + z1 * eyey + z2 * eyez);
                out[15] = 1;

                return out;
            },
            multiply: function (out, a, b) {
                var a00 = a[0], a01 = a[1], a02 = a[2], a03 = a[3];
                var a10 = a[4], a11 = a[5], a12 = a[6], a13 = a[7];
                var a20 = a[8], a21 = a[9], a22 = a[10], a23 = a[11];
                var a30 = a[12], a31 = a[13], a32 = a[14], a33 = a[15];

                var b0 = b[0], b1 = b[1], b2 = b[2], b3 = b[3];
                out[0] = b0 * a00 + b1 * a10 + b2 * a20 + b3 * a30;
                out[1] = b0 * a01 + b1 * a11 + b2 * a21 + b3 * a31;
                out[2] = b0 * a02 + b1 * a12 + b2 * a22 + b3 * a32;
                out[3] = b0 * a03 + b1 * a13 + b2 * a23 + b3 * a33;

                b0 = b[4]; b1 = b[5]; b2 = b[6]; b3 = b[7];
                out[4] = b0 * a00 + b1 * a10 + b2 * a20 + b3 * a30;
                out[5] = b0 * a01 + b1 * a11 + b2 * a21 + b3 * a31;
                out[6] = b0 * a02 + b1 * a12 + b2 * a22 + b3 * a32;
                out[7] = b0 * a03 + b1 * a13 + b2 * a23 + b3 * a33;

                b0 = b[8]; b1 = b[9]; b2 = b[10]; b3 = b[11];
                out[8] = b0 * a00 + b1 * a10 + b2 * a20 + b3 * a30;
                out[9] = b0 * a01 + b1 * a11 + b2 * a21 + b3 * a31;
                out[10] = b0 * a02 + b1 * a12 + b2 * a22 + b3 * a32;
                out[11] = b0 * a03 + b1 * a13 + b2 * a23 + b3 * a33;

                b0 = b[12]; b1 = b[13]; b2 = b[14]; b3 = b[15];
                out[12] = b0 * a00 + b1 * a10 + b2 * a20 + b3 * a30;
                out[13] = b0 * a01 + b1 * a11 + b2 * a21 + b3 * a31;
                out[14] = b0 * a02 + b1 * a12 + b2 * a22 + b3 * a32;
                out[15] = b0 * a03 + b1 * a13 + b2 * a23 + b3 * a33;
                return out;
            },
            identity: function (out) {
                out[0] = 1;
                out[1] = 0;
                out[2] = 0;
                out[3] = 0;
                out[4] = 0;
                out[5] = 1;
                out[6] = 0;
                out[7] = 0;
                out[8] = 0;
                out[9] = 0;
                out[10] = 1;
                out[11] = 0;
                out[12] = 0;
                out[13] = 0;
                out[14] = 0;
                out[15] = 1;
                return out;
            }
        };

        function initVertexBuffers(gl) {
            var latitudeBands = 50;
            var longitudeBands = 50;
            var radius = 2;

            var vertexPositionData = [];
            var normalData = [];
            var textureCoordData = [];
            var indexData = [];

            var latNumber, longNumber;

            // Calculate sphere vertex positions, normals, and texture coordinates.
            for (latNumber = 0; latNumber <= latitudeBands; ++latNumber) {
                var theta = latNumber * Math.PI / latitudeBands;
                var sinTheta = Math.sin(theta);
                var cosTheta = Math.cos(theta);

                for (longNumber = 0; longNumber <= longitudeBands; ++longNumber) {
                    var phi = longNumber * 2 * Math.PI / longitudeBands;
                    var sinPhi = Math.sin(phi);
                    var cosPhi = Math.cos(phi);

                    var x = cosPhi * sinTheta;
                    var y = cosTheta;
                    var z = sinPhi * sinTheta;

                    var u = 1 - (longNumber / longitudeBands);
                    var v = 1 - (latNumber / latitudeBands);

                    vertexPositionData.push(radius * x);
                    vertexPositionData.push(radius * y);
                    vertexPositionData.push(radius * z);

                    normalData.push(x);
                    normalData.push(y);
                    normalData.push(z);

                    textureCoordData.push(u);
                    textureCoordData.push(v);
                }
            }

            // Calculate sphere indices.
            for (latNumber = 0; latNumber < latitudeBands; ++latNumber) {
                for (longNumber = 0; longNumber < longitudeBands; ++longNumber) {
                    var first = (latNumber * (longitudeBands + 1)) + longNumber;
                    var second = first + longitudeBands + 1;

                    indexData.push(first);
                    indexData.push(second);
                    indexData.push(first + 1);

                    indexData.push(second);
                    indexData.push(second + 1);
                    indexData.push(first + 1);
                }
            }

            vertexPositionData = new Float32Array(vertexPositionData);
            normalData = new Float32Array(normalData);
            textureCoordData = new Float32Array(textureCoordData);
            indexData = new Uint16Array(indexData);

            // Create buffer objects.
            var vertexPositionBuffer = gl.createBuffer();
            var vertexNormalBuffer = gl.createBuffer();
            var indexBuffer = gl.createBuffer();

            // Write the vertex positions to their buffer object.
            gl.bindBuffer(gl.ARRAY_BUFFER, vertexPositionBuffer);
            gl.bufferData(gl.ARRAY_BUFFER, vertexPositionData, gl.STATIC_DRAW);

            // Assign position coords to attrib and enable it.
            var VertexPosition = gl.getAttribLocation(program, 'c');
            gl.vertexAttribPointer(VertexPosition, 3, gl.FLOAT, false, 0, 0);
            gl.enableVertexAttribArray(VertexPosition);

            // Write the normals to their buffer object.
            gl.bindBuffer(gl.ARRAY_BUFFER, vertexNormalBuffer);
            gl.bufferData(gl.ARRAY_BUFFER, normalData, gl.STATIC_DRAW);

            // Assign normal to attrib and enable it.
            var VertexNormal = gl.getAttribLocation(program, 'd');
            gl.vertexAttribPointer(VertexNormal, 3, gl.FLOAT, false, 0, 0);
            gl.enableVertexAttribArray(VertexNormal);

            // Pass index buffer data to element array buffer.
            gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, indexBuffer);
            gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, indexData, gl.STATIC_DRAW);

            return indexData.length;
        }

        function generate() {
            if (!(gl = getRenderingContext()))
                return;

            var vertexShader = gl.createShader(gl.VERTEX_SHADER);
            gl.shaderSource(vertexShader, VERTEX_SHADER);
            gl.compileShader(vertexShader);
            var fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
            gl.shaderSource(fragmentShader, FRAGMENT_SHADER);
            gl.compileShader(fragmentShader);
            program = gl.createProgram();
            gl.attachShader(program, vertexShader);
            gl.attachShader(program, fragmentShader);
            gl.linkProgram(program);
            gl.detachShader(program, vertexShader);
            gl.detachShader(program, fragmentShader);
            gl.deleteShader(vertexShader);
            gl.deleteShader(fragmentShader);
            gl.useProgram(program);

            // Init vertex buffers (position, color, and index data).
            var n = initVertexBuffers(gl);

            // Set up clear color and enable depth testing.
            gl.clearColor(0.0, 0.0, 0.0, 1.0);
            gl.enable(gl.DEPTH_TEST);

            // Create projection matrix.
            var projection = mat4.create();
            mat4.perspective(projection, Math.PI / 6, 1.0, 0.1, 100.0);

            // Create model-view matrix.
            var modelView = mat4.create();
            mat4.lookAt(modelView, [0.0, 0.0, 10.0], [0.0, 0.0, 0.0], [0.0, 1.0, 0.0]);

            // Multiply the projection matrix by the model-view matrix to create the mvpMatrix.
            var mvpMatrix = mat4.create();
            mat4.multiply(mvpMatrix, projection, modelView);

            // Pass the modelView matrix into the shader.
            var ModelViewMatrix = gl.getUniformLocation(program, 'h');
            gl.uniformMatrix4fv(ModelViewMatrix, false, modelView);

            // Pass the mvp matrix into the shader.
            var MVP = gl.getUniformLocation(program, 'i');
            gl.uniformMatrix4fv(MVP, false, mvpMatrix);

            // Pass the light position into the shader.
            var LightPosition = gl.getUniformLocation(program, 'e');
            gl.uniform4fv(LightPosition, [10.0, 10.0, 10.0, 1.0]);

            // Pass the material diffuse color into the shader.
            var Kd = gl.getUniformLocation(program, 'f');
            gl.uniform3fv(Kd, [0.9, 0.5, 0.3]);

            // Pass the light diffuse color into the shader.
            var Ld = gl.getUniformLocation(program, 'g');
            gl.uniform3fv(Ld, [1.0, 1.0, 1.0]);

            // Clear & draw.
            gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
            gl.drawElements(gl.TRIANGLES, n, gl.UNSIGNED_SHORT, 0);

            cleanup();
            return canvas.toDataURL();
        }

        function cleanup() {
            gl.useProgram(null);
            if (program)
                gl.deleteProgram(program);
        }

        function getRenderingContext() {
            canvas.width = 67;
            canvas.height = 67;
            var gl = canvas.getContext("webgl")
                || canvas.getContext("experimental-webgl");
            if (gl) {
                gl.viewport(0, 0, 67, 67);
                gl.clearColor(0.0, 0.0, 0.0, 1.0);
                gl.clear(gl.COLOR_BUFFER_BIT);
            }
            return gl;
        }

        var imageHash = 0;

        canvas = document.createElement("canvas");
        if (canvas != null) {
            // Get the image data as a string.
            var imageData = generate();
            if (imageData) {
                // Hash the image data.
                imageHash = fnvHash(imageData);
            }
        }

        return imageHash;
    }
    // Performs an FNV hash on the string provided.
    // @param str the string to be hashed.
    // @return the hash value as a 32 bit integer.
    function fnvHash(str) {
        var h = 0x811c9dc5;
        for (var i = 0; i < str.length; ++i) {
            h ^= str.charCodeAt(i);
            h += (h << 1) + (h << 4) + (h << 7) + (h << 8) + (h << 24);
        }
        return h >>> 0;
    }

    function hash() {

        // Draws an image that changes very subtly based on the GPU model used
        // to render it.
        // @param a canvas instance that has not be drawn to.
        // @return a base 64 encoded string containing the image.
        function drawImage(canvas) {

            // Configure the canvas and get context.
            canvas.width = 67;
            canvas.height = 67;
            var ctx = canvas.getContext('2d', { alpha: true });

            if (ctx != null) {

                // Configure the canvas context.
                ctx.imageSmoothingQuality = "low";
                ctx.imageSmoothingEnabled = true;
                ctx.globalCompositeOperation = "source-over";
                ctx.globalAlpha = 1;
                ctx.miterLimit = Infinity;
                ctx.filter = "none";
                ctx.lineCap = "butt";
                ctx.lineDashOffset = 0;
                ctx.lineJoin = "miter";
                ctx.font = "10pt Arial";
                ctx.lineWidth = 2;
                // setLineDash not supported on iPhone 3G / iOS 4.2
                if (ctx.setLineDash !== undefined) {
                    ctx.setLineDash([10, 20]);
                }
                ctx.shadowColor = "black";
                ctx.shadowOffsetX = -3;
                ctx.shadowOffsetY = -5;

                // Rotate the canvas and add some text.
                ctx.translate(canvas.width / 2, canvas.height / 2);
                ctx.rotate(0.8901179);
                ctx.fillStyle = "green";
                ctx.textAlign = "center";
                ctx.textBaseline = "middle";
                ctx.fillText("*51Degrees*", 0, 0);

                // Draw a transparent circle or elipse over the text. A circle
                // is used if the ellipse feature is not supported by the GPU.
                ctx.beginPath();
                ctx.shadowColor = "yellow";
                ctx.shadowBlur = 1;
                ctx.shadowOffsetX = 1;
                ctx.shadowOffsetY = 1;
                ctx.strokeStyle = "red";
                ctx.fillStyle = "rgba(0, 0, 255, 0.6)";
                if (ctx.ellipse === undefined) {
                    ctx.arc(0, 0, 25, 0, 2 * Math.PI);
                }
                else {
                    ctx.ellipse(0, 0, 25, 15, Math.PI / 4, 0, 2 * Math.PI);
                }
                ctx.fill();
                ctx.stroke();

                // Return the image as a base 64 encoded string.
                return canvas.toDataURL();
            }
        }

        var imageHash = 0;

        var canvas = document.createElement("canvas");
        if (canvas != null) {

            // Get the image data as a string.
            var imageData = drawImage(canvas);
            if (imageData) {

                // Hash the image data.
                imageHash = fnvHash(imageData);
            }
        }

        return imageHash;
    }

    // Try getting the renderer string via the conventional debug extension.
    // @return the UNMASKED_RENDERER_WEBGL parameter value.
    function reportedrenderer() {
        var canvas = document.createElement("canvas");
        if (canvas != null) {
            var context = canvas.getContext("webgl") ||
                canvas.getContext("experimental-webgl");
            if (context) {
                var info = context.getExtension("WEBGL_debug_renderer_info");
                if (info) {
                    return context.getParameter(info.UNMASKED_RENDERER_WEBGL);
                }
            }
        }
        return '';
    }

    // Width of the screen in pixels.
    function width() {
        return window.screen.width * window.devicePixelRatio;
    }

    // Height of the screen in pixels.
    function height() {
        return window.screen.height * window.devicePixelRatio;
    }

    // Pixel ratio of the screen.
    function ratio() {
        return window.devicePixelRatio;
    }

    // Determines if the query is supported by the device.
    // @param query the media query to check
    // @return true if the query is supported, otherwise false
    function hasMediaSupport(query) {
        return window.matchMedia(query).matches;
    }

    // Takes a list of values for a media query name and returns
    // the one that is supported, or undefined if none are supported.
    // @param name of the media query
    // @param possibleValues possible values for the media query name
    // @return the first value from the list that matches, otherwise "n/a"
    function getMediaSingleValue(name, possibleValues) {
        for (var i = 0; i < possibleValues.length; i++) {
            if (hasMediaSupport('(' + name + ': ' + possibleValues[i] + ')')) {
                return possibleValues[i];
            }
        }
        return 'n/a';
    }

    // Returns the color gamut value from the media queries.
    // @return p2 or srgb or undefined
    function mediacolorgamut() {
        return getMediaSingleValue('color-gamut', ['p3', 'srgb']);
    }

    // Returns the portion of the User-Agent which represents the family of 
    // Apple products the device belongs.
    // @return iPhone, iPad or Macintosh otherwise empty string
    function family() {
        var segments = /iPhone|iPad|Macintosh/.exec(navigator.userAgent);
        if (segments && segments.length > 0) {
            return segments[0];
        }
        return '';
    }
    //var previousResults = [];

    function resolveNode(node, value, iterations) {

        // For all the children of the node being evaluated.
        for (var i = 0; i < node.n.length; i++) {

            // Get the values for the child.
            var child = decisionTree.Data[node.n[i]];

            // If the child is a range of values.
            if (child.r) {

                for (var c = 0; c < child.r.length; c++) {
                    var range = child.r[c];

                    // If the value is between the two values as a range.
                    if ((range.a === null || value >= range.a) &&
                        (range.b === null || value <= range.b)) {
                        evaluateNode(child, 0);
                        return;
                    }
                }
            }

            // If the child has a specific list of values.
            else if (child.v) {

                // Compare for equality with the value found.
                if (child.v.indexOf(value) != -1) {
                    evaluateNode(child, 0);
                    return;
                }
            }
        }

        // If this node has children, but we haven't matched any of them, then wait 10ms and 
        // retry the evaluation of this function if we've tried it less than x times.
        if (node.n.length > 0 && iterations < maxRetries) {
            setTimeout(function () {
                evaluateNode(node, iterations + 1)
            }, 10);
        } else {
            //previousResults = [];
            // The result didn't match any children (or there are no children). 
            // Therefore we've reached our result. Return the Profile Id of the current node.
            complete(node.x);
        }
    }

    // Evaluates the children of the node until a Profile Id is found.
    // @param node to be evaluated.
    // @return the profile Id of the best node found.
    function evaluateNode(node, iterations) {

        // If there is a function then execute it, get the result and
        // then find the matching item in the list.
        if (node.m) {

            // Get the value from the function.
            var result = node.m(node);
            // Uncomment the block below to help with debugging the script.
            // Also search for and uncomment other lines containing 'previousResults'
            //if (previousResults.includes(result) === false) {
            //    alert('Function = ' + node.m + '. Iteration = ' + iterations + '. Result = ' + result);
            //}
            //previousResults.push(result);

            // If this isn't a promise then resolve the value returned. If it
            // is a promise it's up to the evaluation method to handle
            // processing the promise once it's resolved.
            if (result || result === '') {
                if (!result.then) {
                    resolveNode(node, result, iterations);
                }
            }
            else if (node.x) {
                complete(node.x);
            }
        }

        // There is no method so return the Profile Id.
        else {
            complete(node.x);
        }
    }

    // Evaluate the root node of the tree.
    evaluateNode(decisionTree.Data[0], 0);
}
const maxRetries = 1;
export {
  getRenderer,
}

