#include <bandit/bandit.h>

extern "C" {
#include "parse.h"
}

using namespace snowhouse;
using namespace bandit;

go_bandit([]() {
        describe("parse", []() {
                it("parsing an empty line", [&]() {
                        char line[] = "";
                        root_t *r = parse(line);
                        AssertThat(r, !IsNull());
                        AssertThat(r->valid, !Equals(0));
                        command_t *c = r->first_command;
                        AssertThat(c, IsNull());
                    });
                it("parsing a line with a single token", [&]() {
                        char line[] = "one";
                        root_t *r = parse(line);
                        AssertThat(r, !IsNull());
                        AssertThat(r->valid, !Equals(0));
                        command_t *c = r->first_command;
                        AssertThat(c, !IsNull());
                        AssertThat(c->argv, !IsNull());
                        AssertThat(c->argc, Equals(2));
                        AssertThat(c->argv[0], !IsNull());
                        AssertThat(c->argv[0], Equals("one"));
                        AssertThat(c->argv[1], IsNull());
                        AssertThat(c->outfile, IsNull());
                        AssertThat(c->next, IsNull());
                    });
                it("parsing a line with a single token surrounded by spaces", [&]() {
                        char line[] = "  one  ";
                        root_t *r = parse(line);
                        AssertThat(r, !IsNull());
                        AssertThat(r->valid, !Equals(0));
                        command_t *c = r->first_command;
                        AssertThat(c, !IsNull());
                        AssertThat(c->argv, !IsNull());
                        AssertThat(c->argc, Equals(2));
                        AssertThat(c->argv[0], !IsNull());
                        AssertThat(c->argv[0], Equals("one"));
                        AssertThat(c->argv[1], IsNull());
                        AssertThat(c->outfile, IsNull());
                        AssertThat(c->next, IsNull());
                    });
                it("parsing a line with a single token surrounded by whitespaces", [&]() {
                        char line[] = "\t\n\r\v\f one ";
                        root_t *r = parse(line);
                        AssertThat(r, !IsNull());
                        AssertThat(r->valid, !Equals(0));
                        command_t *c = r->first_command;
                        AssertThat(c, !IsNull());
                        AssertThat(c->argv, !IsNull());
                        AssertThat(c->argc, Equals(2));
                        AssertThat(c->argv[0], !IsNull());
                        AssertThat(c->argv[0], Equals("one"));
                        AssertThat(c->argv[1], IsNull());
                        AssertThat(c->outfile, IsNull());
                        AssertThat(c->next, IsNull());
                    });
                it("parsing a line with multiple tokens", [&]() {
                        char line[] = "one two three";
                        root_t *r = parse(line);
                        AssertThat(r, !IsNull());
                        AssertThat(r->valid, !Equals(0));
                        command_t *c = r->first_command;
                        AssertThat(c, !IsNull());
                        AssertThat(c->argv, !IsNull());
                        AssertThat(c->argc, Equals(4));
                        AssertThat(c->argv[0], !IsNull());
                        AssertThat(c->argv[0], Equals("one"));
                        AssertThat(c->argv[1], !IsNull());
                        AssertThat(c->argv[1], Equals("two"));
                        AssertThat(c->argv[2], !IsNull());
                        AssertThat(c->argv[2], Equals("three"));
                        AssertThat(c->argv[3], IsNull());
                        AssertThat(c->outfile, IsNull());
                        AssertThat(c->next, IsNull());
                    });
                it("parsing a line with an output redirection", [&]() {
                        char line[] = "one > out";
                        root_t *r = parse(line);
                        AssertThat(r, !IsNull());
                        AssertThat(r->valid, !Equals(0));
                        command_t *c = r->first_command;
                        AssertThat(c, !IsNull());
                        AssertThat(c->argv, !IsNull());
                        AssertThat(c->argc, Equals(2));
                        AssertThat(c->argv[0], !IsNull());
                        AssertThat(c->argv[0], Equals("one"));
                        AssertThat(c->argv[1], IsNull());
                        AssertThat(c->outfile, !IsNull());
                        AssertThat(c->outfile, Equals("out"));
                        AssertThat(c->next, IsNull());
                    });
                it("parsing a line with a pipe", [&]() {
                        char line[] = "one two | three";
                        root_t *r = parse(line);
                        AssertThat(r, !IsNull());
                        AssertThat(r->valid, !Equals(0));
                        command_t *c = r->first_command;
                        AssertThat(c, !IsNull());
                        AssertThat(c->argv, !IsNull());
                        AssertThat(c->argc, Equals(3));
                        AssertThat(c->argv[0], !IsNull());
                        AssertThat(c->argv[0], Equals("one"));
                        AssertThat(c->argv[1], !IsNull());
                        AssertThat(c->argv[1], Equals("two"));
                        AssertThat(c->argv[2], IsNull());
                        AssertThat(c->outfile, IsNull());
                        c = c->next;
                        AssertThat(c, !IsNull());
                        AssertThat(c->argv, !IsNull());
                        AssertThat(c->argc, Equals(2));
                        AssertThat(c->argv[0], !IsNull());
                        AssertThat(c->argv[0], Equals("three"));
                        AssertThat(c->argv[1], IsNull());
                        AssertThat(c->outfile, IsNull());
                        c = c->next;
                        AssertThat(c, IsNull());
                    });
                it("parsing a line with a pipe and output redirections", [&]() {
                        char line[] = "one > out1 | two > out2";
                        root_t *r = parse(line);
                        AssertThat(r, !IsNull());
                        AssertThat(r->valid, !Equals(0));
                        command_t *c = r->first_command;
                        AssertThat(c, !IsNull());
                        AssertThat(c->argv, !IsNull());
                        AssertThat(c->argc, Equals(2));
                        AssertThat(c->argv[0], !IsNull());
                        AssertThat(c->argv[0], Equals("one"));
                        AssertThat(c->argv[1], IsNull());
                        AssertThat(c->outfile, !IsNull());
                        AssertThat(c->outfile, Equals("out1"));
                        c = c->next;
                        AssertThat(c, !IsNull());
                        AssertThat(c->argv, !IsNull());
                        AssertThat(c->argc, Equals(2));
                        AssertThat(c->argv[0], !IsNull());
                        AssertThat(c->argv[0], Equals("two"));
                        AssertThat(c->argv[1], IsNull());
                        AssertThat(c->outfile, !IsNull());
                        AssertThat(c->outfile, Equals("out2"));
                        c = c->next;
                        AssertThat(c, IsNull());
                    });
            });

        describe("parse on malformed input", []() {
                it("parsing a line missing output redirection target", [&]() {
                        char line[] = "one >";
                        root_t *r = parse(line);
                        AssertThat(r, !IsNull());
                        AssertThat(r->valid, Equals(0));
                    });
                it("parsing a line with two output redirections", [&]() {
                        char line[] = "one > >";
                        root_t *r = parse(line);
                        AssertThat(r, !IsNull());
                        AssertThat(r->valid, Equals(0));
                    });
                it("parsing a line with multiple output redirection targets", [&]() {
                        char line[] = "one > out1 out2";
                        root_t *r = parse(line);
                        AssertThat(r, !IsNull());
                        AssertThat(r->valid, Equals(0));
                    });
                it("parsing a line with multiple output redirections", [&]() {
                        char line[] = "one > out1 > out2";
                        root_t *r = parse(line);
                        AssertThat(r, !IsNull());
                        AssertThat(r->valid, Equals(0));
                    });
                it("parsing a line missing command after pipe operator", [&]() {
                        char line[] = "one |";
                        root_t *r = parse(line);
                        AssertThat(r, !IsNull());
                        AssertThat(r->valid, Equals(0));
                    });
                it("parsing a line with two pipe operators", [&]() {
                        char line[] = "one | | two";
                        root_t *r = parse(line);
                        AssertThat(r, !IsNull());
                        AssertThat(r->valid, Equals(0));
                    });
            });
    });
