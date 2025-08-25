#!/bin/perl

$ENV{PATH} = "$ENV{HOME}/bin:$ENV{PATH}";

$template_path = "$ENV{HOME}" . "/.config/git-rewrite-message";

sub read_file {
	my ($filename) = @_;

	$filename = "$template_path" . "/$filename";

	$old_input_rec_sep = $/;
	$/ = undef;
	open(FH, "<", "$filename") or die "failed to open $filename";
	my $content = <FH>;
	close(FH);
	$/ = $old_input_rec_sep;

	return $content;
}

sub save_commit_message {
	my ($filename) = @_;
	
	open(FH, '>', "$filename") or die "Error: $!";
	
	while (<STDIN>) {
		print FH $_;
	}
	close(FH);
}

save_commit_message($ENV{GIT_COMMIT});

# $ENV{GIT_MSG_TEMPLATE} is the template name, such as "baseline", "openeuler"
$template_file = read_file("$ENV{GIT_MSG_TEMPLATE}" . ".template");

eval $template_file;
print "$template";
