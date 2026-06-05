load "./helpers/setup"
load "./helpers/helpers"
load "./helpers/globals"

PROGRAM="$MINIUTILS_BIN/cp"

# Help / Usage

@test "cp -h : prints usage and exits 0" {
    run "$PROGRAM" -h
    assert_success
    assert_output_contains "USAGE"
}

@test "cp --help : prints usage and exits 0" {
    run "$PROGRAM" --help
    assert_success
    assert_output_contains "USAGE"
}

@test "cp : prints usage and exits 0 when no operands" {
    run "$PROGRAM"
    assert_success
    assert_output_contains "usage"
}

@test "cp src : prints usage and exits 0 with one operand" {
    echo hi > src
    run "$PROGRAM" src
    assert_success
    assert_output_contains "usage"
}

@test "cp -x src dst : fails on unknown option" {
    echo hi > src
    run "$PROGRAM" -x src dst
    assert_failure
}

# File → File

@test "cp src dst : creates dst with same content" {
    echo hello > src
    run "$PROGRAM" src dst
    assert_success
    assert_file_exists dst
    assert_file_content_equals dst hello
    assert_file_content_equals src hello
}

@test "cp src dst : overwrites existing file" {
    echo new > src
    echo old > dst
    run "$PROGRAM" src dst
    assert_success
    assert_file_content_equals dst new
}

@test "cp missing dst : fails when source does not exist" {
    run "$PROGRAM" missing dst
    assert_failure
}

@test "cp src missing : fails when destination parent does not exist" {
    echo hi > src
    run "$PROGRAM" src missing/sub/dst
    assert_failure
}

@test "cp src dst : creates empty destination from empty source" {
    : > src
    run "$PROGRAM" src dst
    assert_success
    assert_file_exists dst
    assert_file_empty dst
}

@test "cp src dst : preserves source permissions" {
    echo data > src
    chmod 640 src
    run "$PROGRAM" src dst
    assert_success
    assert_file_mode_equals dst 640
}

# File → Directory

@test "cp src dir/ : copies file into directory" {
    echo hello > myfile
    mkdir dir
    run "$PROGRAM" myfile dir/
    assert_success
    assert_file_exists dir/myfile
    assert_file_content_equals dir/myfile hello
}

@test "cp src dir : copies file into existing directory without trailing slash" {
    echo hello > myfile
    mkdir dir
    run "$PROGRAM" myfile dir
    assert_success
    assert_file_exists dir/myfile
    assert_file_content_equals dir/myfile hello
}

@test "cp src dir : treats nonexistent destination as new filename" {
    echo hello > src
    run "$PROGRAM" src dir
    assert_success
    assert_file_exists dir
    assert_file_content_equals dir hello
}

# Multiple Sources → Directory

@test "cp f1 f2 dir/ : copies multiple files into directory" {
    echo one > f1
    echo two > f2
    mkdir dir
    run "$PROGRAM" f1 f2 dir/
    assert_success
    assert_file_content_equals dir/f1 one
    assert_file_content_equals dir/f2 two
}

@test "cp f1 f2 dir : copies multiple files without trailing slash on directory" {
    echo one > f1
    echo two > f2
    mkdir dir
    run "$PROGRAM" f1 f2 dir
    assert_success
    assert_file_content_equals dir/f1 one
    assert_file_content_equals dir/f2 two
}

@test "cp f1 f2 nonexistent/ : fails when destination directory does not exist" {
    echo one > f1
    echo two > f2
    run "$PROGRAM" f1 f2 nonexistent/
    assert_failure
    assert_output_contains "not a directory"
}

@test "cp f1 f2 dst : fails when destination is a file" {
    echo one > f1
    echo two > f2
    echo block > dst
    run "$PROGRAM" f1 f2 dst
    assert_failure
    assert_output_contains "not a directory"
}

@test "cp f1 missing dir/ : fails when one source is missing" {
    echo one > f1
    mkdir dir
    run "$PROGRAM" f1 missing dir/
    assert_failure
}

# Recursive Directory Copy

@test "cp dir1 dir2 : fails without -r when source is a directory" {
    mkdir dir1
    run "$PROGRAM" dir1 dir2
    assert_failure
    assert_output_contains "is a directory"
}

@test "cp dir1 dir2/ : fails without -r for directory into directory" {
    mkdir dir1 dir2
    run "$PROGRAM" dir1 dir2/
    assert_failure
    assert_output_contains "is a directory"
}

@test "cp dir1 dir2 dir/ : fails without -r when copying directory in multi-source" {
    mkdir dir1 dir2 dir
    run "$PROGRAM" dir1 dir2 dir/
    assert_failure
}

@test "cp -r src dst : creates destination tree when destination does not exist" {
    mkdir -p src/sub
    echo root > src/a.txt
    echo nested > src/sub/b.txt
    run "$PROGRAM" -r src dst
    assert_success
    assert_dir_exists dst
    assert_file_content_equals dst/a.txt root
    assert_file_content_equals dst/sub/b.txt nested
}

@test "cp -r empty dst : creates empty destination directory" {
    mkdir empty
    run "$PROGRAM" -r empty dst
    assert_success
    assert_dir_exists dst
}

@test "cp -r src dst : creates dst/src when destination is an existing directory" {
    mkdir -p src/sub
    echo root > src/a.txt
    echo nested > src/sub/b.txt
    mkdir dst
    run "$PROGRAM" -r src dst
    assert_success
    assert_dir_exists dst/src
    assert_file_content_equals dst/src/a.txt root
    assert_file_content_equals dst/src/sub/b.txt nested
}

@test "cp -r src dst/ : copies into existing directory with trailing slash" {
    mkdir -p src
    echo data > src/a.txt
    mkdir dst
    run "$PROGRAM" -r src dst/
    assert_success
    assert_dir_exists dst/src
    assert_file_content_equals dst/src/a.txt data
}

@test "cp -r src f1 dir/ : copies directory and file in multi-source mode" {
    mkdir -p src
    echo in-src > src/a.txt
    echo in-file > f1
    mkdir dir
    run "$PROGRAM" -r src f1 dir/
    assert_success
    assert_file_content_equals dir/f1 in-file
    assert_dir_exists dir/src
    assert_file_content_equals dir/src/a.txt in-src
}

@test "cp -r missing dst : fails when recursive source does not exist" {
    run "$PROGRAM" -r missing dst
    assert_failure
}

@test "cp -r src parent/child : fails when destination parent is a file" {
    mkdir -p src
    echo data > src/a.txt
    echo block > parent
    run "$PROGRAM" -r src parent/child
    assert_failure
}

@test "cp -r src missing : creates missing destination directory tree" {
    mkdir -p src
    echo data > src/a.txt
    run "$PROGRAM" -r src missing
    assert_success
    assert_dir_exists missing
    assert_file_content_equals missing/a.txt data
}

# Self-Copy

@test "cp file file : fails self-copy without corrupting file" {
    echo hello > file
    run "$PROGRAM" file file
    assert_failure
    assert_output_contains "identical"
    assert_file_content_equals file hello
}

@test "cp -r dir dir : fails recursive self-copy of directory" {
    mkdir -p dir/sub
    echo data > dir/a.txt
    run "$PROGRAM" -r dir dir
    assert_failure
    assert_output_contains "identical"
}

@test "cp -r dir dir/ : fails recursive self-copy into same directory" {
    mkdir -p dir/sub
    echo data > dir/a.txt
    run "$PROGRAM" -r dir dir/
    assert_failure
}

# Edge Cases

@test "cp longpath dst : fails when source path exceeds PATH_MAX" {
    long=$(printf 'a%.0s' {1..1020})
    run "$PROGRAM" "$long" dst
    assert_failure
}

@test "cp -r src dst : fails when destination exists as a regular file" {
    mkdir -p src
    echo data > src/a.txt
    echo block > dst
    run "$PROGRAM" -r src dst
    assert_failure
}

@test "cp -r src dst : overwrites existing file in destination directory" {
    mkdir -p src
    echo new > src/over.txt
    mkdir -p dst
    echo old > dst/over.txt
    run "$PROGRAM" -r src dst
    assert_success
    assert_file_content_equals dst/src/over.txt new
}

@test "cp f1 dir/ : overwrites existing file in destination directory" {
    echo new > f1
    mkdir dir
    echo old > dir/f1
    run "$PROGRAM" f1 dir/
    assert_success
    assert_file_content_equals dir/f1 new
}

@test "cp f1 f2 missing dir/ : fails when any multi-source path is missing" {
    echo one > f1
    mkdir dir
    run "$PROGRAM" f1 missing f2 dir/
    assert_failure
}
