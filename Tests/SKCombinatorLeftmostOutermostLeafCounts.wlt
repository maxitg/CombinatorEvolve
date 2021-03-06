<|
  "SKCombinatorLeftmostOutermostLeafCounts" -> <|
    "tests" -> {
      VerificationTest[
        SKCombinatorLeftmostOutermostLeafCounts[#1, Length[#2] - 1],
        #2
      ] & @@@ {
        {s[s][s][s[s]][s][s], {7, 8, 8, 11, 11, 11, 12, 17, 25, 33, 41, 50, 59, 87, 115, 149, 187,
                               215, 243, 272, 301, 389, 398, 413, 422, 431, 440, 450, 460, 491, 533,
                               575, 623, 697, 739, 781, 824, 867, 997, 1127, 1289, 1451, 1619, 1912,
                               2074, 2236, 2399, 2562, 3052, 3062, 3072, 3088, 3109, 3119, 3129,
                               3140, 3151, 3185, 3231, 3312, 3393, 3480, 3608, 3689, 3770, 3852,
                               3934, 4181, 4192, 4215, 4238, 4267, 4302, 4325, 4348, 4372, 4396,
                               4469, 4542, 4627, 4786, 4945, 5110, 5355, 5514, 5673, 5833, 5993,
                               6474, 6498, 6534, 6595, 6656, 6723, 6821, 6882, 6943, 7005, 7067,
                               7254, 7441, 7628, 7827, 8214, 8601, 8994, 9581, 9968, 10355, 10743,
                               11131, 12296, 12358, 12420, 12494, 12631, 12768, 12911, 13123, 13260,
                               13397, 13535, 13673, 14088, 14503, 14918, 15333, 15760, 16603, 17446,
                               18295, 19566, 20409, 21252, 22096, 22940, 25473, 25611, 25749, 25887,
                               26037, 26326, 26615, 26910, 27350, 27639, 27928, 28218, 28508, 29379,
                               30250, 31121, 31992, 32863, 33746, 35501, 37256, 39017, 41656, 43411,
                               45166, 46922, 48678, 53947, 54237, 54527, 54817, 55107, 55409, 56002,
                               56595, 57194, 58090, 58683, 59276, 59870, 60464, 62247, 64030, 65813,
                               67596, 69379, 71162, 72957, 76536, 80115, 83700, 89075, 92654, 96233,
                               99813, 103393, 114134, 114728, 115322, 115916, 116510, 117104,
                               117710, 118911, 120112}},
        {s[s[s]][s][s[s]][s][k], {8, 9, 11, 11, 16, 22, 22, 35, 35, 48, 62, 62, 68, 68, 79, 79, 90,
                                  102, 108, 115, 115, 130, 146, 146, 162, 194, 160, 160, 160, 160, 168,
                                  153, 206, 260, 260, 314, 422, 312, 328, 345, 377, 444, 375, 375, 375,
                                  375, 383, 368, 468, 569, 569, 670, 872, 668, 668, 668, 668, 676, 661,
                                  677, 694, 694, 711, 745, 709, 651, 651, 651, 651, 659, 644, 711, 779,
                                  779, 847, 983, 845, 826, 826, 826, 826, 834, 819, 851, 884, 884, 917,
                                  983, 915, 915, 915, 915, 923, 908, 924, 941, 941, 958, 992, 956, 898,
                                  898, 898, 898, 906, 891, 995, 1100, 1100, 1205, 1415, 1203, 1236,
                                  1270, 1336, 1471, 1334, 1334, 1334, 1334, 1342, 1327, 1343, 1360,
                                  1360, 1377, 1411, 1375, 1317, 1317, 1317, 1317, 1325, 1310, 1512,
                                  1715, 1715, 1918, 2324, 1916, 1916, 1916, 1916, 1924, 1909, 1925,
                                  1942, 1942, 1959, 1993, 1957, 1899, 1899, 1899, 1899, 1907, 1892,
                                  1925, 1959, 1959, 1993, 2061, 1991, 1882, 1882, 1882, 1882, 1890,
                                  1875, 1891, 1908, 1908, 1925, 1959, 1923, 1865, 1865, 1865, 1865,
                                  1873, 1858, 1993, 2129, 2129, 2265, 2537, 2263, 2227, 2227, 2227,
                                  2227, 2235, 2220, 2236, 2253, 2253, 2270, 2304, 2268, 2210, 2210}},
        {s[s][s][s[s]][s][k[s]], {8, 9, 9, 12, 13, 14, 16, 22, 33, 20, 31, 42, 54, 67, 105, 101, 153,
                                  205, 257, 310, 364, 525, 512, 728, 724, 737, 750, 763, 777, 792, 836,
                                  823, 849, 836, 862, 904, 891, 978, 963, 1120, 1277, 1273, 1444, 1615,
                                  1786, 1958, 2131, 2649, 2636, 3328, 3370, 3366, 3422, 3478, 3534,
                                  3591, 3649, 3822, 3809, 4041, 4026, 4432, 4838, 4834, 5254, 5674,
                                  6094, 6515, 6937, 8202, 8189, 9877, 9864, 9922, 9907, 10023, 10139,
                                  10135, 10265, 10395, 10525, 10656, 10788, 11183, 11170, 11698, 12226,
                                  12213, 12800, 12785, 13901, 15017, 15013, 16143, 17273, 18403, 19534,
                                  20666, 24061, 24048, 28576, 28444, 32972, 32959, 33091, 33223, 33210,
                                  33401, 33386, 33710, 34034, 34030, 34368, 34706, 35044, 35383, 35723,
                                  36742, 36729, 38089, 37957, 40337, 40279, 44020, 43888, 44551, 44211,
                                  44874, 44861, 45201, 45069, 45749, 45691, 46712, 46580, 47243, 46903,
                                  49269, 51635, 51622, 54329, 54197, 59271, 59213, 66995, 66863, 69893,
                                  69553, 72583, 73604, 73591, 74953, 74821, 77205, 77147, 80894, 80762,
                                  81425, 81085, 81748, 81408, 84456, 87504, 87491, 90880, 90748, 97186,
                                  97128, 106956, 106824, 110536, 110196, 113908, 113568, 114231,
                                  114894, 114881, 115885, 115753, 117421, 117363, 120036, 119904,
                                  121930, 121590, 123616, 123484, 125510, 125452, 127817, 127685,
                                  131380, 131367, 135062, 134930, 140652, 140594, 150012, 149880,
                                  157968, 157836}}
      },

      (* the second one overflows and requires GMP *)
      VerificationTest[
        SKCombinatorLeftmostOutermostLeafCounts[s[s][s][s[s]][s][s], 3000],
        SKCombinatorLeftmostOutermostLeafCounts[s[s][s][s[s]][s][s], 4000][[1 ;; 3001]]
      ],

      VerificationTest[
        AllTrue[# > 0 &] @ SKCombinatorLeftmostOutermostLeafCounts[s[s][s][s[s]][s][s], 4000]
      ],

      VerificationTest[
        CombinatorLeftmostOutermostLeafCounts[{e[x_][y_] :> x[x[y]]}, e[e[e][e]][e][e], 1000],
        {6, 8, 8, 9, 9, 15, 20, 20, 24, 27, 27, 28, 28, 31, 33, 33, 34, 34,
         34, 34, 35, 35, 35, 37, 37, 38, 38, 38, 38, 39, 39, 39, 39, 40, 40,
         43, 45, 45, 46, 46, 46, 46, 47, 47, 47, 49, 49, 50, 50, 50, 50, 51,
         51, 51, 54, 54, 55, 55, 58, 60, 60, 61, 61, 61, 61, 62, 62, 62, 64,
         64, 65, 65, 65, 65, 66, 66, 66, 66, 67, 67, 70, 72, 72, 73, 73, 73,
         73, 74, 74, 74, 76, 76, 77, 77, 77, 77, 78, 78, 78, 78, 82, 85, 85,
         86, 86, 89, 91, 91, 92, 92, 92, 92, 93, 93, 93, 95, 95, 96, 96, 96,
         96, 97, 97, 97, 97, 98, 98, 101, 103, 103, 104, 104, 104, 104, 105,
         105, 105, 107, 107, 108, 108, 108, 108, 109, 109, 109, 112, 112, 113,
         113, 116, 118, 118, 119, 119, 119, 119, 120, 120, 120, 122, 122, 123,
         123, 123, 123, 124, 124, 124, 124, 125, 125, 128, 130, 130, 131, 131,
         131, 131, 132, 132, 132, 134, 134, 135, 135, 135, 135, 136, 136, 136,
         141, 141, 145, 148, 148, 149, 149, 152, 154, 154, 155, 155, 155, 155,
         156, 156, 156, 158, 158, 159, 159, 159, 159, 160, 160, 160, 160, 161,
         161, 164, 166, 166, 167, 167, 167, 167, 168, 168, 168, 170, 170, 171,
         171, 171, 171, 172, 172, 172, 175, 175, 176, 176, 179, 181, 181, 182,
         182, 182, 182, 183, 183, 183, 185, 185, 186, 186, 186, 186, 187, 187,
         187, 187, 188, 188, 191, 193, 193, 194, 194, 194, 194, 195, 195, 195,
         197, 197, 198, 198, 198, 198, 199, 199, 199, 199, 203, 206, 206, 207,
         207, 210, 212, 212, 213, 213, 213, 213, 214, 214, 214, 216, 216, 217,
         217, 217, 217, 218, 218, 218, 218, 219, 219, 222, 224, 224, 225, 225,
         225, 225, 226, 226, 226, 228, 228, 229, 229, 229, 229, 230, 230, 230,
         233, 233, 234, 234, 237, 239, 239, 240, 240, 240, 240, 241, 241, 241,
         243, 243, 244, 244, 244, 244, 245, 245, 245, 245, 246, 246, 249, 251,
         251, 252, 252, 252, 252, 253, 253, 253, 255, 255, 256, 256, 256, 256,
         257, 257, 257}
      ],

      VerificationTest[
        CombinatorLeftmostOutermostLeafCounts[{e[x_][y_] :> x[y[e][e]]}, e[e[e][e]][e][e], 200],
        {6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
         24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
         41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
         58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74,
         75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91,
         92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106,
         107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120,
         121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134,
         135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148,
         149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162,
         163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176,
         177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190,
         191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204,
         205, 206}
      ]
    }
  |>
|>
