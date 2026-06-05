load "./helpers/setup"
load "./helpers/helpers"
load "./helpers/globals"

PROGRAM="$MINIUTILS_BIN/cat"

# Help / Usage

@test "cat -h : prints usage and exits 0" {
    run "$PROGRAM" -h
    assert_success
    assert_output_contains "USAGE"
}

@test "cat --help : prints usage and exits 0" {
    run "$PROGRAM" --help
    assert_success
    assert_output_contains "USAGE"
}

# Single File

@test "cat file : prints file contents to stdout" {
    echo hello > file
    run "$PROGRAM" file
    assert_success
    assert_output_equals hello
}

@test "cat empty_file : exits 0 with no output" {
    : > empty_file
    run "$PROGRAM" empty_file
    assert_success
    assert_output_empty
}

@test "cat large_file : prints full contents beyond 4096 bytes" {
    printf 'x%.0s' {1..5000} > large_file
    run "$PROGRAM" large_file
    assert_success
    assert_output_equals "$(printf 'x%.0s' {1..5000})"
}

@test "cat binary_file : prints contents byte-for-byte" {
    printf '\x01\x02\xff\xfeABC' > binary_file
    run "$PROGRAM" binary_file
    assert_success
    printf '%s' "$output" > actual
    cmp -s binary_file actual
}

# Multiple Files

@test "cat file1 file2 : concatenates files in order" {
    echo one > file1
    echo two > file2
    run "$PROGRAM" file1 file2
    assert_success
    assert_output_equals $'one\ntwo'
}

@test "cat file1 file2 file3 : concatenates all files in order" {
    echo one > file1
    echo two > file2
    echo three > file3
    run "$PROGRAM" file1 file2 file3
    assert_success
    assert_output_equals $'one\ntwo\nthree'
}

# Stdin

@test "cat stdin : prints piped input" {
    run bash -c "echo hello | '$PROGRAM'"
    assert_success
    assert_output_equals hello
}

@test "cat empty stdin : exits 0 with no output" {
    run bash -c "printf '' | '$PROGRAM'"
    assert_success
    assert_output_empty
}

# Error Cases

@test "cat nonexistent : fails when file does not exist" {
    run "$PROGRAM" nonexistent
    assert_failure
}

@test "cat dir/ : fails when operand is a directory" {
    mkdir dir
    run "$PROGRAM" dir/
    assert_failure
}

@test "cat file1 nonexistent file2 : fails after outputting prior files" {
    echo one > file1
    echo two > file2
    run "$PROGRAM" file1 nonexistent file2
    assert_failure
    assert_output_contains "$(cat file1)"
}

# Edge Cases

@test "cat file : preserves content without trailing newline" {
    printf 'no newline' > file
    run "$PROGRAM" file
    assert_success
    assert_output_equals 'no newline'
}

@test "cat file : prints exactly 4096 bytes" {
    printf 'b%.0s' {1..4096} > file
    run "$PROGRAM" file
    assert_success
    assert_output_equals "$(printf 'b%.0s' {1..4096})"
}

@test "cat file : prints exactly 4097 bytes" {
    printf 'c%.0s' {1..4097} > file
    run "$PROGRAM" file
    assert_success
    assert_output_equals "$(printf 'c%.0s' {1..4097})"
}
