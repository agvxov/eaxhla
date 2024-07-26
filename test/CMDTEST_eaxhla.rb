$default_output_file = "a.out"

class CMDTEST_error_batch < Cmdtest::Testcase
  def test_unknown_instruction
    import_file "test/unknown_instruction.eax", "./"

    cmd "eaxhla unknown_instruction.eax" do
      stderr_equal /.+/
      exit_status 1
    end
  end

  def test_unclosed_comment
    import_file "test/unclosed_comment.eax", "./"

    cmd "eaxhla unclosed_comment.eax" do
      stderr_equal /.+/
      exit_status 1
    end
  end

  def test_unclosed_artimetric
    import_file "test/unclosed_artimetric.eax", "./"

    cmd "eaxhla unclosed_artimetric.eax" do
      stderr_equal /.+/
      exit_status 1
    end
  end

  def test_unclosed_program
    import_file "test/unclosed_program.eax", "./"

    cmd "eaxhla unclosed_program.eax" do
      stderr_equal /.+/
      exit_status 1
    end
  end

  def test_double_declare
    import_file "test/double_declare.eax", "./"

    cmd "eaxhla double_declare.eax" do
      stderr_equal /.+/
      exit_status 1
    end
  end

  def test_double_program
    import_file "test/double_program.eax", "./"

    cmd "eaxhla double_program.eax" do
      stderr_equal /.+/
      exit_status 1
    end
  end

  def test_cut_string
    import_file "test/cut_string.eax", "./"

    cmd "eaxhla cut_string.eax" do
      stderr_equal /.+\n(.|\n)+/m
      exit_status 1
    end
  end

  def test_multi_error
    import_file "test/multi_error.eax", "./"

    cmd "eaxhla multi_error.eax" do
      stderr_equal /.+\n(.|\n)+/m
      exit_status 1
    end
  end
end

class CMDTEST_warning_batch < Cmdtest::Testcase
  def test_overflows
    import_file "test/overflows.eax", "./"

    ignore_file $default_output_file

    cmd "eaxhla overflows.eax" do
      stderr_equal /.+/
    end
  end

  def test_very_empty
    import_file "test/very_empty.eax", "./"

    ignore_file $default_output_file

    cmd "eaxhla very_empty.eax" do
      stderr_equal /.+/
    end
  end

  def test_empty
    import_file "test/empty.eax", "./"

    ignore_file $default_output_file

    cmd "eaxhla empty.eax" do
      stderr_equal /.+/
    end
  end
end
