/*
 * (C) Copyright 2001-2004
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2002
 * David Mueller, ELSOFT AG, d.mueller@elsoft.ch
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/* This code should work for both the S3C2400 and the S3C2410
 * as they seem to have the same PLL and clock machinery inside.
 * The different address mapping is handled by the s3c24xx.h files below.
 */

#include <common.h>
#ifdef CONFIG_S3C24X0

#include <asm/io.h>
#include <asm/arch/s3c24x0_cpu.h>

#define MPLL 0
#define UPLL 1

/* ------------------------------------------------------------------------- */
/* NOTE: This describes the proper use of this file.
 *
 * CONFIG_SYS_CLK_FREQ should be defined as the input frequency of the PLL.
 *
 * get_FCLK(), get_HCLK(), get_PCLK() and get_UCLK() return the clock of
 * the specified bus in HZ.
 */
/* ------------------------------------------------------------------------- */

static ulong get_PLLCLK(int pllreg)
{
	struct s3c24x0_clock_power *clk_power = s3c24x0_get_base_clock_power();
	ulong r, m, p, s;
	ulong  ret;

	if (pllreg == MPLL)
		r = readl(&clk_power->MPLLCON);
	else if (pllreg == UPLL)
		r = readl(&clk_power->UPLLCON);
	else
		hang();

	m = ((r & 0xFF000) >> 12) + 8;
	p = ((r & 0x003F0) >> 4) + 2;
	s = r & 0x3;
	
#if defined(CONFIG_S3C2440)
    if (pllreg == MPLL)
    {
	    //extern void debug_led(); debug_led();
        //参考S3C2440 芯片手册上的公式：PLL=(2 * m * Fin)/(p * 2s)
        ret = (CONFIG_SYS_CLK_FREQ * m * 2) / (p << s);   
        return ret;		
    }
	else if (pllreg == UPLL)
#endif

	return (CONFIG_SYS_CLK_FREQ * m) / (p << s);
}

/* return FCLK frequency */
ulong get_FCLK(void)
{
	return get_PLLCLK(MPLL);
}

/* return HCLK frequency */
ulong get_HCLK(void)
{
	struct s3c24x0_clock_power *clk_power = s3c24x0_get_base_clock_power();

#if defined(CONFIG_S3C2440)
    //return(get_FCLK()/4);
	#if 1
	if (readl(&clk_power->CLKDIVN)&0x6)
	{
        if ((readl(&clk_power->CLKDIVN)&0x6) == 2)
	    {
	        return (get_FCLK()/2);
	    }
	    if ((readl(&clk_power->CLKDIVN)&0x6) == 4)
	    {
	        return ((readl(&clk_power->CAMDIVN)&0x200)?get_FCLK()/8:get_FCLK()/4);
	    }
	    if ((readl(&clk_power->CLKDIVN)&0x6) == 6)
	    {
	        return ((readl(&clk_power->CAMDIVN)&0x100)?get_FCLK()/6:get_FCLK()/3);
	    }
		return(get_FCLK());
	}
	else
	{
	    return(get_FCLK());
	}
	#endif
#else
	return (readl(&clk_power->CLKDIVN) & 2) ? get_FCLK() / 2 : get_FCLK();
#endif
}

/* return PCLK frequency */
ulong get_PCLK(void)
{
	struct s3c24x0_clock_power *clk_power = s3c24x0_get_base_clock_power();

	return (readl(&clk_power->CLKDIVN) & 1) ? get_HCLK() / 2 : get_HCLK();
}

/* return UCLK frequency */
ulong get_UCLK(void)
{
	return get_PLLCLK(UPLL);
}

#endif /* CONFIG_S3C24X0 */
