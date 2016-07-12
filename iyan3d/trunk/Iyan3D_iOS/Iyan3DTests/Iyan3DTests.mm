//
//  Iyan3DTests.m
//  Iyan3DTests
//
//  Created by sabish on 12/07/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import <XCTest/XCTest.h>

#include "Mat4.h"
#include "Mat4GLK.h"

@interface Iyan3DTests : XCTestCase

@end

@implementation Iyan3DTests

- (void)setUp {
    [super setUp];
}

- (void)tearDown {
    [super tearDown];
}

- (void)testMat4Initialize {
    Mat4 m;
    
    float idm1[] = {
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    };
    Mat4 m1 = Mat4(idm1);
    
    XCTAssertTrue(m == m1);

    float r1[] = {
        0.0, 0.0, 0.0, 1.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        1.0, 0.0, 0.0, 0.0
    };
    Mat4 m2 = Mat4(r1);
    
    XCTAssertFalse(m == m2);
}

- (void)testMat4Multiplication1 {
    float idm1[] = {
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    };
    Mat4 m = Mat4(idm1);
    
    float r2[] = {
        2.0, 2.0, 2.0, 2.0,
        2.0, 2.0, 2.0, 2.0,
        2.0, 2.0, 2.0, 2.0,
        2.0, 2.0, 2.0, 2.0
    };
    Mat4 m1 = Mat4(r2);
    
    XCTAssertTrue(m1 == (m1 * m));

    XCTAssertTrue(m1 == (m * m1));
}

- (void)testMat4Multiplication2 {
    float idm1[] = {
        1.5, 1.5, 1.5, 1.5,
        1.5, 1.5, 1.5, 1.5,
        1.5, 1.5, 1.5, 1.5,
        1.5, 1.5, 1.5, 1.5
    };
    Mat4 m = Mat4(idm1);
    
    float r1[] = {
        2.5, 2.5, 2.5, 2.5,
        2.5, 2.5, 2.5, 2.5,
        2.5, 2.5, 2.5, 2.5,
        2.5, 2.5, 2.5, 2.5
    };
    Mat4 m1 = Mat4(r1);

    float r2[] = {
        15.0, 15.0, 15.0, 15.0,
        15.0, 15.0, 15.0, 15.0,
        15.0, 15.0, 15.0, 15.0,
        15.0, 15.0, 15.0, 15.0
    };
    Mat4 m2 = Mat4(r2);

    XCTAssertTrue(m2 == (m1 * m));
}

- (void)testMat4Multiplication3 {
    float ra[] = {
        3.6, 6.6, 2.7, 9.8,
        4.5, 7.5, 3.6, 8.9,
        5.4, 8.4, 4.5, 7.9,
        6.3, 9.3, 5.4, 6.8
    };
    Mat4 A = Mat4(ra);
    
    float rb[] = {
        4.5, 7.5, 3.6, 8.9,
        6.3, 9.3, 5.4, 6.8,
        5.4, 8.4, 4.5, 7.9,
        3.6, 6.6, 2.7, 9.8
    };
    Mat4 B = Mat4(rb);

    float r2[] = {
        107.639999,	175.740005,	87.209999,	194.290009,
        118.979996,	192.479996,	96.929993,	206.710007,
        129.960007,	208.559998,	106.380005,	218.149994,
        140.580002,	223.980011,	115.560005,	228.610001
    };
    Mat4 C1 = Mat4(r2);

    Mat4 t1 = B * A;
    XCTAssertTrue(C1 == t1);
    
    float r3[] = {
        125.459999,	198.959991,	103.409996,	199.809998,
        136.529999,	219.930008,	111.510002,	233.410004,
        131.309999,	209.910004,	107.729996,	216.949997,
        118.979999,	187.080002,	98.550003,	181.989990
    };
    Mat4 C2 = Mat4(r3);

    Mat4 t2 = A * B;
    XCTAssertTrue(C2 == t2);
}

- (void)testPerformanceMat4 {
    [self measureBlock:^{
        Mat4 r;
        for (int i = 0; i < 9999; i++) {
            Mat4 m1 = getRandomMatrix();
            for (int j = 0; j < 999; j++) {
                Mat4 m2 = getRandomMatrix();
                r *= (m1 * m2);
            }
        }
    }];
}

     Mat4 getRandomMatrix() {
         Mat4 m1;
         for (int i = 0; i < 16; i++)
             m1.setElement((float)rand() / (float)RAND_MAX, i);
         return m1;
     }
@end
