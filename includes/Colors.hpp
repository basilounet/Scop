//
// Created by bvasseur on 9/7/25.
//

#ifndef SCOP_COLORS_HPP
# define SCOP_COLORS_HPP

# define RESET	"\033[0m"
# define CLR	"\033[0m"
# define BOLD	"\033[1m"
# define UNDL	"\033[4m"
# define DUNDL	"\033[21m"
# define ITAL	"\033[3m"
# define STRK	"\033[9m"

# define BLK	"\033[0;30m"
# define RED	"\033[0;31m"
# define GRN	"\033[0;32m"
# define YLW	"\033[0;33m"
# define BLU	"\033[0;34m"
# define PRP	"\033[0;35m"
# define CYN	"\033[0;36m"
# define WHT	"\033[0;37m"
# define ORG RGB(255, 92, 0)

# define RGB(r, g, b) "\033[0;38;2;" #r ";" #g ";" #b "m"
# define DSTR RGB(200, 200, 0)

# define AND <<
# define PRINT std::cout AND
# define PRERR std::cerr AND
# define ENDL AND std::endl
# define CENDL AND CLR ENDL
# define TAB "\t" AND
# define NEWL PRINT "" ENDL;
# define SEMAND AND "; " AND
# define SANDN(name) AND "; " #name ":" AND

#endif //SCOP_COLORS_HPP