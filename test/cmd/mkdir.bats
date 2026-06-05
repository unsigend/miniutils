load "./helpers/setup"
load "./helpers/helpers"
load "./helpers/globals"

PROGRAM="$MINIUTILS_BIN/mkdir"

# Help / Usage

@test "mkdir -h : prints usage and exits 0" {
    run "$PROGRAM" -h
    assert_success
    assert_output_contains "USAGE"
}

@test "mkdir --help : prints usage and exits 0" {
    run "$PROGRAM" --help
    assert_success
    assert_output_contains "USAGE"
}

@test "mkdir : prints usage and exits 0 when no operands" {
    run "$PROGRAM"
    assert_success
    assert_output_contains "usage"
}

# Basic Directory Creation

@test "mkdir dir : creates directory" {
    run "$PROGRAM" dir
    assert_success
    assert_dir_exists dir
}

@test "mkdir dir1 dir2 dir3 : creates multiple directories" {
    run "$PROGRAM" dir1 dir2 dir3
    assert_success
    assert_dir_exists dir1
    assert_dir_exists dir2
    assert_dir_exists dir3
}

@test "mkdir existing_dir : fails when directory already exists" {
    mkdir existing_dir
    run "$PROGRAM" existing_dir
    assert_failure
}

@test "mkdir nonexistent/parent/dir : fails when parent is missing" {
    run "$PROGRAM" nonexistent/parent/dir
    assert_failure
}

# -p

@test "mkdir -p a/b/c : creates intermediate directories" {
    run "$PROGRAM" -p a/b/c
    assert_success
    assert_dir_exists a
    assert_dir_exists a/b
    assert_dir_exists a/b/c
}

@test "mkdir -p existing_dir : succeeds when directory already exists" {
    mkdir existing_dir
    run "$PROGRAM" -p existing_dir
    assert_success
    assert_dir_exists existing_dir
}

@test "mkdir -p a/b/c a/b/d : creates shared parents and both leaf dirs" {
    run "$PROGRAM" -p a/b/c a/b/d
    assert_success
    assert_dir_exists a/b
    assert_dir_exists a/b/c
    assert_dir_exists a/b/d
}

# -m

@test "mkdir -m 700 dir : creates directory with mode 0700" {
    umask 000
    run "$PROGRAM" -m 700 dir
    assert_success
    assert_dir_exists dir
    assert_file_mode_equals dir 700
}

@test "mkdir -m 755 dir : creates directory with mode 0755" {
    umask 000
    run "$PROGRAM" -m 755 dir
    assert_success
    assert_dir_exists dir
    assert_file_mode_equals dir 755
}

@test "mkdir -m 000 dir : creates directory with mode 0000" {
    umask 000
    run "$PROGRAM" -m 000 dir
    assert_success
    assert_dir_exists dir
    assert_file_mode_equals dir 0
    chmod 700 dir
}

@test "mkdir -m 777 dir : creates directory with mode 0777" {
    umask 000
    run "$PROGRAM" -m 777 dir
    assert_success
    assert_dir_exists dir
    assert_file_mode_equals dir 777
}

@test "mkdir -p -m 700 a/b/c : creates nested directory with mode 0700" {
    umask 000
    run "$PROGRAM" -p -m 700 a/b/c
    assert_success
    assert_dir_exists a/b/c
    assert_file_mode_equals a/b/c 700
}

# Mode Validation

@test "mkdir -m 999 dir : fails on invalid octal digit" {
    run "$PROGRAM" -m 999 dir
    assert_failure
}

@test "mkdir -m 8888 dir : fails on invalid octal" {
    run "$PROGRAM" -m 8888 dir
    assert_failure
}

@test "mkdir -m abc dir : fails on non-numeric mode" {
    run "$PROGRAM" -m abc dir
    assert_failure
}

@test "mkdir -m 10000 dir : fails when mode exceeds 07777" {
    run "$PROGRAM" -m 10000 dir
    assert_failure
}

@test "mkdir -m dir : fails create when mode argument is missing" {
    run "$PROGRAM" -m dir
    assert_success
    assert_dir_not_exists dir
}

# Error Cases

@test "mkdir -p : prints usage and exits 0 with no directory operands" {
    run "$PROGRAM" -p
    assert_success
    assert_output_contains "usage"
}

@test "mkdir dir : fails when operand is an existing file" {
    echo data > dir
    run "$PROGRAM" dir
    assert_failure
}
