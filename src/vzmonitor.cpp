/**
 * main source file for vzmonitor
 *
 * @package vzmonitor
 * @copyright Copyright (c) 2015, Matthias Behr
 * @license http://www.gnu.org/licenses/gpl.text GNU Public License V2
 * @author Matthias Behr < mbehr (a) mcbehr.de >
 */

#include <stdio.h>

#include "config.hpp"
#include "gitSha1.h"

int main(int argc, char *argv[]) {

	printf("%s version %s\n", PACKAGE, g_GIT_SHALONG);
	return 0;
}
