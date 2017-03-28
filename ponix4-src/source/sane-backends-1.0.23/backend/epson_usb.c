#include <sys/types.h>
#include "../include/sane/sanei_usb.h"
#include "epson_usb.h"

/* generated with epson2usb.pl doc/descriptions/epson2.desc */

SANE_Word sanei_epson_usb_product_ids[] = {
  0x101, /* GT-7000U, Perfection 636U */
  0x103, /* GT-6600U, Perfection 610 */
  0x104, /* GT-7600U, GT-7600UF, Perfection 1200U, Perfection 1200U PHOTO */
  0x105, /* Stylus Scan 2000 */
  0x106, /* Stylus Scan 2500 */
  0x107, /* ES-2000, Expression 1600 */
  0x109, /* ES-8500, Expression 1640XL */
  0x10a, /* GT-8700, GT-8700F, Perfection 1640SU, Perfection 1640SU PHOTO */
  0x10b, /* GT-7700U, Perfection 1240U */
  0x10c, /* GT-6700U, Perfection 640U */
  0x10e, /* ES-2200, Expression 1680 */
  0x110, /* GT-8200U, GT-8200UF, Perfection 1650, Perfection 1650 PHOTO */
  0x112, /* GT-9700F, Perfection 2450 PHOTO */
  0x11b, /* GT-9300UF, Perfection 2400 PHOTO */
  0x11c, /* GT-9800F, Perfection 3200 PHOTO */
  0x11e, /* GT-8300UF, Perfection 1660 PHOTO */
  0x126, /* ES-7000H, GT-15000 */
  0x128, /* GT-X700, Perfection 4870 PHOTO */
  0x129, /* ES-10000G, Expression 10000XL */
  0x12a, /* GT-X800, Perfection 4990 PHOTO */
  0x12b, /* ES-H300, GT-2500 */
  0x12c, /* GT-X900, Perfection V700 Photo, Perfection V750 Photo */
  0x135, /* GT-X970 */
  0x801, /* CC-600PX, Stylus CX5100, Stylus CX5200 */
  0x802, /* CC-570L, Stylus CX3100, Stylus CX3200 */
  0x805, /* Stylus CX6300, Stylus CX6400 */
  0x806, /* PM-A850, Stylus Photo RX600 */
  0x807, /* Stylus Photo RX500, Stylus Photo RX510 */
  0x808, /* Stylus CX5300, Stylus CX5400 */
  0x80d, /* Stylus CX4500, Stylus CX4600 */
  0x80e, /* PX-A550, Stylus CX3500, Stylus CX3600, Stylus CX3650 */
  0x80f, /* Stylus Photo RX420, Stylus Photo RX425, Stylus Photo RX430 */
  0x810, /* PM-A900, Stylus Photo RX700 */
  0x811, /* PM-A870, Stylus Photo RX620, Stylus Photo RX630 */
  0x813, /* Stylus CX6500, Stylus CX6600 */
  0x814, /* PM-A700 */
  0x815, /* AcuLaser CX11, AcuLaser CX11NF, LP-A500 */
  0x817, /* LP-M5500, LP-M5500F */
  0x818, /* Stylus CX3700, Stylus CX3800, Stylus DX3800 */
  0x819, /* PX-A650, Stylus CX4700, Stylus CX4800, Stylus DX4800, Stylus DX4850 */
  0x81a, /* PM-A750, Stylus Photo RX520, Stylus Photo RX530 */
  0x81c, /* PM-A890, Stylus Photo RX640, Stylus Photo RX650 */
  0x81d, /* PM-A950 */
  0x81f, /* Stylus CX7700, Stylus CX7800 */
  0x820, /* Stylus CX4100, Stylus CX4200, Stylus DX4200 */
  0x827, /* PM-A820, Stylus Photo RX560, Stylus Photo RX580, Stylus Photo RX590 */
  0x828, /* PM-A970 */
  0x829, /* PM-T990 */
  0x82a, /* PM-A920 */
  0x82b, /* Stylus CX4900, Stylus CX5000, Stylus DX5000 */
  0x82e, /* PX-A720, Stylus CX5900, Stylus CX6000, Stylus DX6000 */
  0x82f, /* PX-A620, Stylus CX3900, Stylus DX4000 */
  0x830, /* ME 200, Stylus CX2800, Stylus CX2900 */
  0x833, /* LP-M5600 */
  0x834, /* LP-M6000 */
  0x835, /* AcuLaser CX21 */
  0x836, /* PM-T960 */
  0x837, /* PM-A940, Stylus Photo RX680, Stylus Photo RX685, Stylus Photo RX690 */
  0x838, /* PX-A640, Stylus CX7300, Stylus CX7400, Stylus DX7400 */
  0x839, /* PX-A740, Stylus CX8300, Stylus CX8400, Stylus DX8400 */
  0x83a, /* PX-FA700, Stylus CX9300F, Stylus CX9400Fax, Stylus DX9400F */
  0x83c, /* PM-A840, PM-A840S, Stylus Photo RX585, Stylus Photo RX595, Stylus Photo RX610 */
  0x841, /* ME 300, PX-401A, Stylus NX100, Stylus SX100, Stylus TX100 */
  0x843, /* LP-M5000 */
  0x844, /* Artisan 800, EP-901A, EP-901F, Stylus Photo PX800FW, Stylus Photo TX800FW */
  0x846, /* Artisan 700, EP-801A, Stylus Photo PX700W, Stylus Photo TX700W */
  0x847, /* ME Office 700FW, PX-601F, Stylus Office BX600FW, Stylus Office TX600FW, Stylus SX600FW, WorkForce 600 */
  0x848, /* ME Office 600F, Stylus Office BX300F, Stylus Office TX300F, Stylus NX300 */
  0x849, /* Stylus NX200, Stylus SX200, Stylus SX205, Stylus TX200, Stylus TX203, Stylus TX209 */
  0x84a, /* PX-501A, Stylus NX400, Stylus SX400, Stylus SX405, Stylus TX400 */
  0x84c, /* WorkForce 500 */
  0x84d, /* PX-402A, Stylus NX110 Series, Stylus SX110 Series, Stylus TX110 Series */
  0x84f, /* ME OFFICE 510, Stylus NX210 Series, Stylus SX210 Series, Stylus TX210 Series */
  0x851, /* Stylus NX410 Series, Stylus SX410 Series, Stylus TX410 Series */
  0x854, /* ME OFFICE 650FN Series, Stylus Office BX310FN Series, Stylus Office TX510FN Series, WorkForce 310 Series */
  0x856, /* PX-502A, Stylus NX510 Series, Stylus SX510W Series, Stylus TX550W Series */
  0x85c, /* Stylus SX125 */
  0				/* last entry - this is used for devices that are specified 
				   in the config file as "usb <vendor> <product>" */
};

int
sanei_epson_getNumberOfUSBProductIds (void)
{
  return sizeof (sanei_epson_usb_product_ids) / sizeof (SANE_Word);
}
