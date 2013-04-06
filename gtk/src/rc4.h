/*
 ** Copyright (©) 2003-2013 Teus Benschop.
 **  
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 3 of the License, or
 ** (at your option) any later version.
 **  
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **  
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */

// The code was copied from software copyrighted (©) 2006 Olivier Gay <olivier.gay@a3.epfl.ch>


#ifndef RC4_H
#define RC4_H

typedef unsigned char uint8;
typedef unsigned int uint32;

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8 se[256], sd[256];
    uint32 pose, posd;
    uint8 te, td;
} rc4_ctx;

void rc4_ks(rc4_ctx *ctx, const uint8 *key, uint32 key_len);
void rc4_encrypt(rc4_ctx *ctx, const uint8 *src, uint8 *dst, uint32 len);
void rc4_decrypt(rc4_ctx *ctx, const uint8 *src, uint8 *dst, uint32 len);


#ifdef __cplusplus
}
#endif

#endif /* !RC4_H */

