$default_output_file = "test_me_please"

class CMDTEST_error_batch < Cmdtest::Testcase
  def test_unknown_instruction
    create_file "input.eax", <<-HEREDOC
      procedure a
          nop
      end procedure

      unix
      program main
          wigglecall a
      end program
    HEREDOC

    cmd "eaxhla input.eax" do
      stderr_equal /.+/
      exit_status 1
    end
  end

  def test_unclosed_comment
    create_file "input.eax", <<-HEREDOC
      /*
          reeeeeeee
          reeeeeeeee
          reeeeee
    HEREDOC

    cmd "eaxhla input.eax" do
      stderr_equal /.+/
      exit_status 1
    end
  end

  def test_unclosed_artimetric
    create_file "input.eax", <<-HEREDOC
      program a
          u8 var = [
    HEREDOC

    cmd "eaxhla input.eax" do
      stderr_equal /.+/
      exit_status 1
    end
  end

  def test_unclosed_program
    create_file "input.eax", <<-HEREDOC
      unix
      program main
      begin
          exit 1
      end rpogram
    HEREDOC

    cmd "eaxhla input.eax" do
      stderr_equal /.+/
      exit_status 1
    end
  end

  def test_double_declare
    create_file "input.eax", <<-HEREDOC
      program main
          u8 a
          u8 a
      begin
      end program
    HEREDOC

    cmd "eaxhla input.eax" do
      stderr_equal /.+/
      exit_status 1
    end
  end

  def test_double_program
    create_file "input.eax", <<-HEREDOC
      program a
      begin
      end program

      program b
      begin
      end program
    HEREDOC

    cmd "eaxhla input.eax" do
      stderr_equal /.+/
      exit_status 1
    end
  end

  def test_multi_error
    create_file "input.eax", <<-HEREDOC
      program main
          k8 kek
      begin
          wigglecall func
          xor $rsp rsp
          xor rsp rsp
          poke rsp
          xor rsp rsp
          xor rsp rsp
          xor rsp rsp
      end program
    HEREDOC

    cmd "eaxhla input.eax" do
      stderr_equal /.+\n(.|\n)+/m
      exit_status 1
    end
  end
end

class CMDTEST_warning_batch < Cmdtest::Testcase
  def test_overflows
    create_file "input.eax", <<-HEREDOC
      program main
          u8 a = 10
          u8 b = 10000
          u8 c = -200
      begin
      end program
    HEREDOC

    ignore_file $default_output_file

    cmd "eaxhla input.eax" do
      stderr_equal /.+/
    end
  end

  def test_empty
    create_file "input.eax", <<-HEREDOC
    HEREDOC

    ignore_file $default_output_file

    cmd "eaxhla input.eax" do
      stderr_equal /.+/
    end
  end
end
