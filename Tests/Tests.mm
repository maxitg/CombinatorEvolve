#import <XCTest/XCTest.h>

#include "../libCombinatorEvolve/CombinatorSystem.hpp"

@interface Tests : XCTestCase

@end

@implementation Tests
- (void)setUp {}

- (void)tearDown {}

const auto shouldAbort = [](){ return false; };

const CombinatorEvolve::CombinatorRules skRules =
    {{{8, 10}, {9, 10}, {-3, 8}, {-2, 8}, {0, 1}, {2, 9}, {3, 9}, {6, 10}}, {{7, 4}, {5, 8}}};

- (void)testASimpleRule {
  CombinatorEvolve::CombinatorSystem system({{-3, -3}, {0, -3}}, 1);
  XCTAssertEqual(system.evolve(skRules, 1, shouldAbort), 1);
  XCTAssertEqual(system.leafCounts(), std::vector<uint64_t>({3, 1}));
}

void testLeafCounts(const std::vector<CombinatorEvolve::CombinatorExpression>& initialExpressions,
                    const CombinatorEvolve::ExpressionID initialRoot,
                    const std::vector<uint64_t>& expectedAnswer) {
  CombinatorEvolve::CombinatorSystem system(initialExpressions, initialRoot);

  XCTAssertEqual(system.evolve(skRules, expectedAnswer.size() - 1, shouldAbort), expectedAnswer.size() - 1);

  const auto leafCounts = system.leafCounts();
  const auto leafCountsMPZ = system.leafCountsMPZ();
  XCTAssertEqual(leafCounts.size(), expectedAnswer.size());
  XCTAssertEqual(leafCountsMPZ.size(), expectedAnswer.size());
  for (int i = 0; i < expectedAnswer.size(); ++i) {
    XCTAssertEqual(leafCounts[i], expectedAnswer[i]);
    XCTAssertEqual(leafCountsMPZ[i].get_ui(), expectedAnswer[i]);
  }
}

- (void)testLeafCounts {
  testLeafCounts({{-2, -2}, {0, -2}, {1, 0}, {2, -2}, {3, -2}},
                 4,
                 {7, 8, 8, 11, 11, 11, 12, 17, 25, 33, 41, 50, 59, 87, 115, 149, 187,
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
                  117710, 118911, 120112});
  testLeafCounts({{-2, -2}, {-2, 0}, {1, -2}, {2, 0}, {3, -2}, {4, -3}},
                 5,
                 {8, 9, 11, 11, 16, 22, 22, 35, 35, 48, 62, 62, 68, 68, 79, 79, 90,
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
                  2227, 2235, 2220, 2236, 2253, 2253, 2270, 2304, 2268, 2210, 2210});
  testLeafCounts({{-3, -2}, {-2, -2}, {1, -2}, {2, 1}, {3, -2}, {4, 0}},
                 5,
                 {8, 9, 9, 12, 13, 14, 16, 22, 33, 20, 31, 42, 54, 67, 105, 101, 153,
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
                  157968, 157836});
}

- (void)testOverflow {
  CombinatorEvolve::CombinatorSystem system({{-2, -2}, {0, -2}, {1, 0}, {2, -2}, {3, -2}}, 4);

  XCTAssertEqual(system.evolve(skRules, 3000, shouldAbort), 3000);
  XCTAssertNoThrow(system.leafCounts());

  XCTAssertEqual(system.evolve(skRules, 1000, shouldAbort), 1000);
  XCTAssertThrows(system.leafCounts());
  XCTAssertNoThrow(system.leafCountsMPZ());
}

- (void)testPositive {
  CombinatorEvolve::CombinatorSystem system({{-2, -2}, {0, -2}, {1, 0}, {2, -2}, {3, -2}}, 4);

  XCTAssertEqual(system.evolve(skRules, 1500, shouldAbort), 1500);

  const auto leafCounts = system.leafCountsMPZ();
  for (int i = 0; i < 1500; ++i) {
    XCTAssertGreaterThan(leafCounts[i], 0);
  }
}

- (void)testNoRules {
  CombinatorEvolve::CombinatorSystem system({}, -2);
  XCTAssertEqual(system.evolve(CombinatorEvolve::CombinatorRules({}, {}), 5, shouldAbort), 0);
  XCTAssertEqual(system.leafCounts(), std::vector<uint64_t>({1}));
}

- (void)testFlippingRule {
  CombinatorEvolve::CombinatorSystem system({{-2, -3}}, 0);
  XCTAssertEqual(system.evolve({{{2, 3}, {3, 2}}, {{0, 1}}}, 5, shouldAbort), 5);
  XCTAssertEqual(system.leafCounts(), std::vector<uint64_t>({2, 2, 2, 2, 2, 2}));
}

CombinatorEvolve::CombinatorSystem runEvolution(
    const std::vector<CombinatorEvolve::CombinatorExpression>& initialExpressions,
    const CombinatorEvolve::ExpressionID initialRoot,
    const int stepCount) {
  CombinatorEvolve::CombinatorSystem system(initialExpressions, initialRoot);
  system.evolve(skRules, stepCount, shouldAbort);
  return system;
}

- (void)testEvolutionPerformance1 {
  [self measureBlock:^{
    runEvolution({{-2, -2}, {0, -2}, {1, 0}, {2, -2}, {3, -2}}, 4, 1500);
  }];
}

- (void)testLeafCountingPerformance1 {
  const auto system = runEvolution({{-2, -2}, {0, -2}, {1, 0}, {2, -2}, {3, -2}}, 4, 1500);
  [self measureBlock:^{
    system.leafCounts();
  }];
}

- (void)testEvolutionPerformance2 {
  [self measureBlock:^{
    runEvolution({{-2, -2}, {-2, 0}, {1, -2}, {2, 0}, {3, -2}, {4, -3}}, 5, 20000);
  }];
}

- (void)testLeafCountingPerformance2 {
  const auto system = runEvolution({{-2, -2}, {-2, 0}, {1, -2}, {2, 0}, {3, -2}, {4, -3}}, 5, 20000);
  [self measureBlock:^{
    system.leafCounts();
  }];
}

- (void)testEvolutionPerformance3 {
  [self measureBlock:^{
    runEvolution({{-3, -2}, {-2, -2}, {1, -2}, {2, 1}, {3, -2}, {4, 0}}, 5, 3000);
  }];
}

- (void)testLeafCountingPerformance3 {
  const auto system = runEvolution({{-3, -2}, {-2, -2}, {1, -2}, {2, 1}, {3, -2}, {4, 0}}, 5, 3000);
  [self measureBlock:^{
    system.leafCounts();
  }];
}

@end
