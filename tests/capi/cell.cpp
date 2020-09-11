// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <cmath>
#include <array>

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.h"

static bool approx_eq(chfl_vector3d lhs, std::array<double, 3> rhs, double eps = 1e-12) {
    return (fabs(lhs[0] - rhs[0]) < eps) && (fabs(lhs[1] - rhs[1]) < eps) && (fabs(lhs[2] - rhs[2]) < eps);
}

static bool approx_eq(chfl_vector3d lhs[3], chfl_vector3d rhs[3], double eps = 1e-12) {
    return
        (fabs(lhs[0][0] - rhs[0][0]) < eps) && (fabs(lhs[0][1] - rhs[0][1]) < eps) && (fabs(lhs[0][2] - rhs[0][2]) < eps) &&
        (fabs(lhs[1][0] - rhs[1][0]) < eps) && (fabs(lhs[1][1] - rhs[1][1]) < eps) && (fabs(lhs[1][2] - rhs[1][2]) < eps) &&
        (fabs(lhs[2][0] - rhs[2][0]) < eps) && (fabs(lhs[2][1] - rhs[2][1]) < eps) && (fabs(lhs[2][2] - rhs[2][2]) < eps);
}

TEST_CASE("chfl_cell") {
    SECTION("Constructors") {
        chfl_vector3d lengths = {2, 3, 4};
        CHFL_CELL* cell = chfl_cell(lengths, nullptr);
        REQUIRE(cell);

        chfl_vector3d data = {0};
        CHECK_STATUS(chfl_cell_lengths(cell, data));
        CHECK(approx_eq(data, {{2, 3, 4}}));

        CHECK_STATUS(chfl_cell_angles(cell, data));
        CHECK(approx_eq(data, {{90, 90, 90}}));

        chfl_cellshape shape;
        CHECK_STATUS(chfl_cell_shape(cell, &shape));
        CHECK(shape == CHFL_CELL_ORTHORHOMBIC);

        chfl_free(cell);

        lengths[0] = 20; lengths[1] = 21; lengths[2] = 22;
        chfl_vector3d angles = {90, 100, 120};
        cell = chfl_cell(lengths, angles);
        REQUIRE(cell);

        CHECK_STATUS(chfl_cell_lengths(cell, data));
        CHECK(approx_eq(data, {{20, 21, 22}}));

        CHECK_STATUS(chfl_cell_angles(cell, data));
        CHECK(approx_eq(data, {{90, 100, 120}}));

        CHECK_STATUS(chfl_cell_shape(cell, &shape));
        CHECK(shape == CHFL_CELL_TRICLINIC);

        chfl_free(cell);

        chfl_vector3d matrix[3] = {
            {5, 0, 0},
            {0, 3, 0},
            {0, 0, 8}
        };
        cell = chfl_cell_from_matrix(matrix);
        REQUIRE(cell);

        CHECK_STATUS(chfl_cell_lengths(cell, data));
        CHECK(approx_eq(data, {{5, 3, 8}}));

        CHECK_STATUS(chfl_cell_angles(cell, data));
        CHECK(approx_eq(data, {{90, 90, 90}}));

        CHECK_STATUS(chfl_cell_shape(cell, &shape));
        CHECK(shape == CHFL_CELL_ORTHORHOMBIC);

        chfl_free(cell);
    }

    SECTION("Constructors errors") {
        chfl_vector3d dummy = {0, 0, 0};
        fail_next_allocation();
        CHECK(chfl_cell(dummy, nullptr) == nullptr);

        fail_next_allocation();
        chfl_vector3d matrix[3] = {
            {5, 0, 0},
            {0, 3, 0},
            {0, 0, 8}
        };
        CHECK(chfl_cell_from_matrix(matrix) == nullptr);

        CHFL_CELL* cell = chfl_cell(dummy, nullptr);
        REQUIRE(cell);

        fail_next_allocation();
        CHECK(chfl_cell_copy(cell) == nullptr);

        CHFL_FRAME* frame = chfl_frame();
        REQUIRE(frame);

        fail_next_allocation();
        CHECK(chfl_cell_from_frame(frame) == nullptr);

        chfl_free(cell);
        chfl_free(frame);
    }

    SECTION("copy") {
        chfl_vector3d lengths = {2, 2, 2};
        CHFL_CELL* cell = chfl_cell(lengths, nullptr);
        REQUIRE(cell);

        CHFL_CELL* copy = chfl_cell_copy(cell);
        REQUIRE(copy);

        double volume = 0;
        CHECK_STATUS(chfl_cell_volume(cell, &volume));
        CHECK(volume == 8);

        CHECK_STATUS(chfl_cell_volume(copy, &volume));
        CHECK(volume == 8);

        chfl_vector3d new_lengths = {3, 3, 3};
        CHECK_STATUS(chfl_cell_set_lengths(cell, new_lengths));

        CHECK_STATUS(chfl_cell_volume(cell, &volume));
        CHECK(volume == 27);

        CHECK_STATUS(chfl_cell_volume(copy, &volume));
        CHECK(volume == 8);

        chfl_free(copy);
        chfl_free(cell);
    }

    SECTION("Length") {
        chfl_vector3d lengths = {2, 3, 4};
        CHFL_CELL* cell = chfl_cell(lengths, nullptr);
        REQUIRE(cell);

        chfl_vector3d data = {0};
        CHECK_STATUS(chfl_cell_lengths(cell, data));
        CHECK(data[0] == 2);
        CHECK(data[1] == 3);
        CHECK(data[2] == 4);

        lengths[0] = 10; lengths[1] = 20; lengths[2] = 30;
        CHECK_STATUS(chfl_cell_set_lengths(cell, lengths));
        CHECK_STATUS(chfl_cell_lengths(cell, data));
        CHECK(data[0] == 10);
        CHECK(data[1] == 20);
        CHECK(data[2] == 30);

        chfl_free(cell);
    }

    SECTION("Angles") {
        chfl_vector3d lengths = {2, 3, 4};
        CHFL_CELL* cell = chfl_cell(lengths, nullptr);
        REQUIRE(cell);

        chfl_vector3d data = {0};
        CHECK_STATUS(chfl_cell_angles(cell, data));
        CHECK(data[0] == 90);
        CHECK(data[1] == 90);
        CHECK(data[2] == 90);

        chfl_vector3d angles = {80, 89, 100};
        // Setting an orthorhombic cell angles is an error
        CHECK(chfl_cell_set_angles(cell, angles) != CHFL_SUCCESS);

        CHECK_STATUS(chfl_cell_set_shape(cell, CHFL_CELL_TRICLINIC));

        CHECK_STATUS(chfl_cell_set_angles(cell, angles));
        CHECK_STATUS(chfl_cell_angles(cell, data));
        CHECK(data[0] == 80);
        CHECK(data[1] == 89);
        CHECK(data[2] == 100);

        chfl_free(cell);
    }

    SECTION("Volume") {
        chfl_vector3d lengths = {2, 3, 4};
        CHFL_CELL* cell = chfl_cell(lengths, nullptr);
        REQUIRE(cell);

        double volume = 0;
        CHECK_STATUS(chfl_cell_volume(cell, &volume));
        CHECK(volume == 2 * 3 * 4);

        chfl_free(cell);
    }

    SECTION("Matrix") {
        chfl_vector3d lengths = {10, 20, 30};
        CHFL_CELL* cell = chfl_cell(lengths, nullptr);
        REQUIRE(cell);

        chfl_vector3d expected[3] = {{10, 0, 0}, {0, 20, 0}, {0, 0, 30}};
        chfl_vector3d matrix[3];
        CHECK_STATUS(chfl_cell_matrix(cell, matrix));
        CHECK(approx_eq(expected, matrix));

        chfl_free(cell);
    }

    SECTION("Shape") {
        CHFL_CELL* cell = chfl_cell(nullptr, nullptr);
        REQUIRE(cell);

        chfl_cellshape shape;
        CHECK_STATUS(chfl_cell_shape(cell, &shape));
        CHECK(shape == CHFL_CELL_INFINITE);

        chfl_vector3d lengths = {2, 3, 4};
        CHECK_STATUS(chfl_cell_set_shape(cell, CHFL_CELL_ORTHORHOMBIC));
        CHECK_STATUS(chfl_cell_set_lengths(cell, lengths));

        CHECK_STATUS(chfl_cell_shape(cell, &shape));
        CHECK(shape == CHFL_CELL_ORTHORHOMBIC);

        CHECK_STATUS(chfl_cell_set_shape(cell, CHFL_CELL_TRICLINIC));
        CHECK_STATUS(chfl_cell_shape(cell, &shape));
        CHECK(shape == CHFL_CELL_TRICLINIC);

        chfl_free(cell);
    }

    SECTION("Wrap") {
        chfl_vector3d lengths = {2, 3, 4};
        CHFL_CELL* cell = chfl_cell(lengths, nullptr);
        REQUIRE(cell);

        chfl_vector3d vector = {0.8, 1.7, -6};
        CHECK_STATUS(chfl_cell_wrap(cell, vector));
        CHECK(vector[0] == 0.8);
        CHECK(vector[1] == -1.3);
        CHECK(vector[2] == 2);

        chfl_free(cell);
    }
}
