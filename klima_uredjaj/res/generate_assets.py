# Generise PNG resurse za 2D projekat "Klima-uredjaj".
# Pre pokretanja upisi svoje podatke u STUDENT_LINE_1 / STUDENT_LINE_2.
from PIL import Image, ImageDraw, ImageFont
import math, os

OUT = os.path.dirname(os.path.abspath(__file__))
STUDENT_LINE_1 = "Ime Prezime"
STUDENT_LINE_2 = "SV 15/2026"
FONT_BOLD = "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf"

def bez(p0, p1, p2, p3, n=40):
    pts = []
    for i in range(n + 1):
        t = i / n
        u = 1 - t
        x = u**3*p0[0] + 3*u*u*t*p1[0] + 3*u*t*t*p2[0] + t**3*p3[0]
        y = u**3*p0[1] + 3*u*u*t*p1[1] + 3*u*t*t*p2[1] + t**3*p3[1]
        pts.append((x, y))
    return pts

# ---------- 1. KURSOR: daljinski, laserska tackica u gornjem levom uglu ----------
def make_cursor(size=64, ss=8):
    S = size * ss
    img = Image.new("RGBA", (S, S), (0, 0, 0, 0))
    L, W = 50 * ss, 16 * ss
    strip = Image.new("RGBA", (L, W), (0, 0, 0, 0))
    d = ImageDraw.Draw(strip)
    d.rounded_rectangle([0, 0, L - 1, W - 1], radius=W // 3, fill=(45, 48, 56, 255), outline=(15, 15, 20, 255), width=ss)
    d.rectangle([0, W // 3, 4 * ss, 2 * W // 3], fill=(120, 125, 135, 255))          # emiter
    d.rounded_rectangle([9 * ss, 4 * ss, 20 * ss, W - 4 * ss], radius=ss, fill=(120, 200, 150, 255))  # LCD
    cy = W // 2
    d.ellipse([24 * ss, cy - 2.6 * ss, 24 * ss + 5.2 * ss, cy + 2.6 * ss], fill=(230, 50, 50, 255))   # power
    for i in range(3):
        x = 32 * ss + i * 6 * ss
        d.ellipse([x, cy - 2.2 * ss, x + 4.4 * ss, cy + 2.2 * ss], fill=(190, 195, 205, 255))
    rot = strip.rotate(-45, resample=Image.BICUBIC, expand=True)
    cx = cy_ = 13 * ss + (L / 2) * math.cos(math.radians(45))
    img.alpha_composite(rot, (int(cx - rot.width / 2), int(cy_ - rot.height / 2)))
    d = ImageDraw.Draw(img)
    e = 13 * ss
    dot = 5 * ss
    d.line([(dot, dot), (e, e)], fill=(255, 40, 40, 200), width=int(1.6 * ss))      # zrak
    r = 3.6 * ss
    d.ellipse([dot - r * 1.6, dot - r * 1.6, dot + r * 1.6, dot + r * 1.6], fill=(255, 60, 60, 90))  # sjaj
    d.ellipse([dot - r, dot - r, dot + r, dot + r], fill=(255, 30, 30, 255))
    d.ellipse([dot - r / 2.5, dot - r / 2.5, dot + r / 2.5, dot + r / 2.5], fill=(255, 220, 220, 255))
    img = img.resize((size, size), Image.LANCZOS)
    img.save(f"{OUT}/cursor_remote.png")

# ---------- 2. IKONICE za treci ekranic ----------
def make_flame(ss=4, size=128):
    S = size * ss
    img = Image.new("RGBA", (S, S), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    anchor = (64, 118)
    def outline(s):
        pts = (bez((64, 6), (72, 36), (112, 60), (96, 100)) +
               bez((96, 100), (96, 130), (32, 130), (32, 100)) +
               bez((32, 100), (16, 60), (56, 40), (64, 6)))
        return [((anchor[0] + (x - anchor[0]) * s) * ss, (anchor[1] + (y - anchor[1]) * s) * ss) for x, y in pts]
    for s, col in [(1.0, (225, 45, 25, 255)), (0.75, (255, 140, 0, 255)), (0.45, (255, 225, 90, 255))]:
        d.polygon(outline(s), fill=col)
    img.resize((size, size), Image.LANCZOS).save(f"{OUT}/icon_fire.png")

def make_snow(ss=4, size=128):
    S = size * ss
    img = Image.new("RGBA", (S, S), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    c = S / 2
    col = (170, 225, 255, 255)
    def seg(p, q, w):
        d.line([p, q], fill=col, width=int(w))
        for a in (p, q):
            d.ellipse([a[0] - w / 2, a[1] - w / 2, a[0] + w / 2, a[1] + w / 2], fill=col)
    R = 58 * ss
    for k in range(6):
        a = math.radians(90 + 60 * k)
        ux, uy = math.cos(a), -math.sin(a)
        tip = (c + ux * R, c + uy * R)
        seg((c, c), tip, 5 * ss)
        for frac, bl in [(0.55, 22 * ss), (0.8, 14 * ss)]:
            bx, by = c + ux * R * frac, c + uy * R * frac
            for sgn in (-1, 1):
                b = a + sgn * math.radians(55)
                seg((bx, by), (bx + math.cos(b) * bl, by - math.sin(b) * bl), 4 * ss)
    d.ellipse([c - 6 * ss, c - 6 * ss, c + 6 * ss, c + 6 * ss], fill=(235, 248, 255, 255))
    img.resize((size, size), Image.LANCZOS).save(f"{OUT}/icon_snow.png")

def make_check(ss=4, size=128):
    S = size * ss
    img = Image.new("RGBA", (S, S), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    pts = [(22 * ss, 68 * ss), (50 * ss, 96 * ss), (106 * ss, 34 * ss)]
    w = 20 * ss
    col = (46, 204, 113, 255)
    d.line(pts, fill=col, width=w, joint="curve")
    for p in (pts[0], pts[2]):
        d.ellipse([p[0] - w / 2, p[1] - w / 2, p[0] + w / 2, p[1] + w / 2], fill=col)
    img.resize((size, size), Image.LANCZOS).save(f"{OUT}/icon_check.png")

# ---------- 3. ATLAS CIFARA: 0-9 - ° C (bele, boje se u sejderu) ----------
def make_atlas(cw=64, ch=96):
    chars = "0123456789-\u00b0C"
    img = Image.new("RGBA", (cw * len(chars), ch), (255, 255, 255, 0))
    d = ImageDraw.Draw(img)
    f = ImageFont.truetype(FONT_BOLD, 72)
    for i, c in enumerate(chars):
        d.text((i * cw + cw / 2, ch / 2), c, font=f, fill=(255, 255, 255, 255), anchor="mm")
    img.save(f"{OUT}/atlas_digits.png")
    with open(f"{OUT}/atlas_digits.txt", "w", encoding="utf-8") as fh:
        fh.write(f"Redosled znakova (levo -> desno): {chars}\n")
        fh.write(f"Velicina celije: {cw}x{ch} px, ukupno {len(chars)} celija\n")
        fh.write("UV za znak i: u0 = i/13, u1 = (i+1)/13, v od 0 do 1\n")
        fh.write("Indeksi: 0-9 = cifre, 10 = '-', 11 = stepen, 12 = 'C'\n")

# ---------- 4. ZNACKA SA IMENOM (poluprovidna) ----------
def make_tag(w=512, h=128):
    img = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.rounded_rectangle([0, 0, w - 1, h - 1], radius=18, fill=(0, 0, 0, 140), outline=(255, 255, 255, 120), width=2)
    f1 = ImageFont.truetype(FONT_BOLD, 46)
    f2 = ImageFont.truetype(FONT_BOLD, 38)
    d.text((w / 2, h * 0.33), STUDENT_LINE_1, font=f1, fill=(255, 255, 255, 235), anchor="mm")
    d.text((w / 2, h * 0.72), STUDENT_LINE_2, font=f2, fill=(255, 255, 255, 235), anchor="mm")
    img.save(f"{OUT}/student_tag.png")

if __name__ == "__main__":
    make_cursor(); make_flame(); make_snow(); make_check(); make_atlas(); make_tag()
    print("Gotovo.")
